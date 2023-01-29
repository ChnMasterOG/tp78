/********************************** (C) COPYRIGHT *******************************
 * File Name          : RF_PHY.c
 * Author             : ChnMasterOG, WCH
 * Version            : V1.0
 * Date               : 2022/11/13
 * Description        : RF 2.4GӦ�ó���
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "HAL.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
tmosTaskID RFTaskId;
BOOL RF_Ready = FALSE;
uint8_t JUMP_BOOT_DATA[1] = {0x7A}; // �涨����0x7A��������BOOT
uint8_t TX_DATA[2] = {0xff, 0xff};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*******************************************************************************
* Function Name  : FLASH_Read_DeviceID
* Description    : ��Flash��ȡRF��BLEѡ��(1-RF, 0-BLE)
* Input          : None
* Return         : None
*******************************************************************************/
void FLASH_Read_RForBLE(void)
{
  EEPROM_READ( FLASH_ADDR_RForBLE, &RF_Ready, 1 );
}

/*******************************************************************************
* Function Name  : FLASH_Write_RForBLE
* Description    : ��RF��BLEѡ��д��Flash
* Input          : rf_ready: �Ƿ�ѡ��RF
* Return         : None
*******************************************************************************/
void FLASH_Write_RForBLE(uint8_t rf_ready)
{
  uint8_t check;
  do {
    EEPROM_WRITE( FLASH_ADDR_RForBLE, &rf_ready, 1 );
    EEPROM_READ( FLASH_ADDR_RForBLE, &check, 1 );
  } while (check != rf_ready);
}

/*********************************************************************
 * @fn      RF_2G4StatusCallBack
 *
 * @brief   RF ״̬�ص���ע�⣺�����ڴ˺�����ֱ�ӵ���RF���ջ��߷���API����Ҫʹ���¼��ķ�ʽ����
 *
 * @param   sta     - ״̬����
 * @param   crc     - crcУ����
 * @param   rxBuf   - ����bufָ��
 *
 * @return  none
 */
void RF_2G4StatusCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf)
{
    switch(sta)
    {
        case TX_MODE_TX_FINISH:
        {
            break;
        }
        case TX_MODE_TX_FAIL:
        {
            break;
        }
        case TX_MODE_RX_DATA:
        {
            RF_Shut();
            if(crc == 1)
            {
                PRINT("crc error\n");
            }
            else if(crc == 2)
            {
                PRINT("match type error\n");
            }
            else
            {
                PRINT("tx recv,rssi:%d\n", (int8_t)rxBuf[0]);
                if (rxBuf[1] == 2 && rxBuf[2] == 0x3) {
                  RF_CapsLock_LEDOn = rxBuf[3];
                }
            }
            break;
        }
        case TX_MODE_RX_TIMEOUT: // Timeout is about 200us
        {
            break;
        }
        case RX_MODE_RX_DATA:
        {
            if(crc == 1)
            {
                PRINT("crc error\n");
            }
            else if(crc == 2)
            {
                PRINT("match type error\n");
            }
            else
            {
                PRINT("rx recv, rssi: %d\n", (int8_t)rxBuf[0]);
                if (rxBuf[1] == 2 && rxBuf[2] == 0x3) {
                  RF_CapsLock_LEDOn = rxBuf[3];
                }
            }
            tmos_set_event(RFTaskId, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FINISH:
        {
            tmos_set_event(RFTaskId, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FAIL:
        {
            break;
        }
    }
    PRINT("STA: %x\n", sta);
}

/*********************************************************************
 * @fn      RF_ProcessEvent
 *
 * @brief   RF �¼�����
 *
 * @param   task_id - ����ID
 * @param   events  - �¼���־
 *
 * @return  δ����¼�
 */
uint16_t RF_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if (events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(task_id)) != NULL)
        {
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
//    if (events & SBP_RF_START_DEVICE_EVT)
//    {
//        tmos_start_task(RFTaskId, SBP_RF_PERIODIC_EVT, 1000);
//        return events ^ SBP_RF_START_DEVICE_EVT;
//    }
//    if (events & SBP_RF_PERIODIC_EVT)
//    {
//        RF_Shut();
//        TX_DATA[0]+=2;
//        RF_Tx(TX_DATA, 10, 0xFF, 0xFF);
//        tmos_start_task(RFTaskId, SBP_RF_PERIODIC_EVT, 1000);
//        return events ^ SBP_RF_PERIODIC_EVT;
//    }
    if (events & SBP_RF_RF_RX_EVT)
    {
        uint8_t state;
        RF_Shut();
        state = RF_Rx(TX_DATA, 2, 0xFF, 0xFF);
        PRINT("RX mode.state = %x\n", state);
        return events ^ SBP_RF_RF_RX_EVT;
    }
    if (events & SBP_RF_MOUSE_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(HIDMouse - 1, HID_MOUSE_DATA_LENGTH + 1, 0xFF, 0xFF);
        return events ^ SBP_RF_MOUSE_REPORT_EVT;
    }
    if (events & SBP_RF_KEYBOARD_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(HIDKey - 1, HID_KEYBOARD_DATA_LENGTH + 1, 0xFF, 0xFF);
        return events ^ SBP_RF_KEYBOARD_REPORT_EVT;
    }
    if (events & SBP_RF_VOL_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(HIDVolume - 1, HID_VOLUME_DATA_LENGTH + 1, 0xFF, 0xFF);
        return events ^ SBP_RF_VOL_REPORT_EVT;
    }
    if (events & SBP_RF_JUMPBOOT_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(JUMP_BOOT_DATA, 1, 0xFF, 0xFF);
        return events ^ SBP_RF_JUMPBOOT_REPORT_EVT;
    }
    return 0;
}

/*********************************************************************
 * @fn      RF_Init
 *
 * @brief   RF ��ʼ��
 *
 * @return  none
 */
void RF_Init(void)
{
    uint8_t    state;
    rfConfig_t rfConfig;

    tmos_memset(&rfConfig, 0, sizeof(rfConfig_t));
    RFTaskId = TMOS_ProcessEventRegister(RF_ProcessEvent);
    rfConfig.accessAddress = 0x71764129; // ��ֹʹ��0x55555555�Լ�0xAAAAAAAA ( ���鲻����24��λ��ת���Ҳ�����������6��0��1 )
    rfConfig.CRCInit = 0x555555;
    rfConfig.Channel = 8;
    rfConfig.Frequency = 2420000;
    rfConfig.LLEMode = LLE_MODE_AUTO | LLE_MODE_EX_CHANNEL; // ʹ�� LLE_MODE_EX_CHANNEL ��ʾ ѡ�� rfConfig.Frequency ��Ϊͨ��Ƶ��
    rfConfig.rfStatusCB = RF_2G4StatusCallBack;
    rfConfig.RxMaxlen = 251;
    state = RF_Config(&rfConfig);
    PRINT("rf 2.4g init: %x\n", state);
}

/******************************** endfile @ main ******************************/
