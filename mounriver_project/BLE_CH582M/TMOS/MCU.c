/********************************** (C) COPYRIGHT *******************************
 * File Name          : MCU.c
 * Author             : ChnMasterOG, WCH
 * Version            : V1.2
 * Date               : 2022/1/26
 * Description        : Ӳ������������BLE��Ӳ����ʼ��
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CH58x_common.h"
#include "HAL.h"
#include <string.h>

/* �ʵ� */
#include "snake.h"

/*HID data*/
UINT8 HID_DATA[HID_DATA_LENGTH] = { 0x0, 0x0, 0x0, 0x0, 0x0,
                                    0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                                    0x2, 0x0 }; // bit1~bit4: mouse data, bit6~bit13: key data, bit15: vol data
/*����������*/
UINT8* HIDMouse = &HID_DATA[1];
UINT8* HIDKey = &HID_DATA[6];
/*������������*/
UINT8* HIDVolume = &HID_DATA[15];

tmosTaskID halTaskID = INVALID_TASK_ID;
BOOL USB_CapsLock_LEDOn = FALSE, BLE_CapsLock_LEDOn = FALSE, RF_CapsLock_LEDOn = FALSE;
BOOL priority_USB = TRUE;   // USB������/RFͬʱ����ѡ��
BOOL enable_BLE = TRUE;   // ʹ�ܻ�ʧ������

static BOOL CapsLock_LEDOn_state = FALSE; // Caps Lock LED ON/OFF
static BOOL connection_state[3] = { FALSE, FALSE, FALSE };  // USB/BLE/RF state
static uint32_t EP_counter = 0;  // �ʵ�������

/*******************************************************************************
* Function Name  : HID_KEYBOARD_Process
* Description    : ��ѭ�����̴�����
* Input          : ��
* Return         : ��
*******************************************************************************/
__attribute__((weak)) void HID_KEYBOARD_Process(void)
{
  uint8_t res;
  KEYBOARD_Detection();
  if (KEYBOARD_data_ready != 0) {    // ���ͼ�������
    KEYBOARD_data_ready = 0;
    if ( EnterPasskey_flag == TRUE ) { // ���������������
      res = KEYBOARD_EnterPasskey( &BLE_Passkey );
      if ( res == 0 ) {
        EnterPasskey_flag = FALSE;
        tmos_start_task( hidEmuTaskId, START_SEND_PASSKEY_EVT, 400 );
      }
    }
    else {
      if ( KEYBOARD_Custom_Function() ) { // ����Fn��������Ϣ�򲻲��������¼�
        if ( USB_Ready == TRUE && priority_USB == TRUE ) {
          tmos_set_event( usbTaskID, USB_KEYBOARD_EVENT );  // USB�����¼�
        } else if ( BLE_Ready == TRUE ) {
          tmos_set_event( hidEmuTaskId, START_KEYBOARD_REPORT_EVT );  // ���������¼�
        } else if ( RF_Ready == TRUE ) {
          tmos_set_event( RFTaskId, SBP_RF_KEYBOARD_REPORT_EVT );  // RF�����¼�
        }
      }
      if (KEYBOARD_mouse_ready != 0) { // ���ͼ����������
        KEYBOARD_mouse_ready = 0;
        tmos_memset(&HIDMouse[1], 0, 3);   // ֻ�������Ҽ�û����������
        if ( USB_Ready == TRUE && priority_USB == TRUE ) {
          tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB����¼�
        } else if ( BLE_Ready == TRUE ) {
          tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //��������¼�
        } else if ( RF_Ready == TRUE ) {
          tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF�����¼�
        }
      }
    }
  }
}

/*******************************************************************************
* Function Name  : HID_PS2TP_Process
* Description    : PS2С��㴦����
* Input          : ��
* Return         : ��
*******************************************************************************/
__attribute__((weak)) void HID_PS2TP_Process(void)
{
  if (PS2_data_ready != 0 && enable_TP == TRUE) {    // ����С����������
    PS2_data_ready = 0;
    if ( PS2_byte_cnt == 3 ) {  // ���������ݱ�
      PS2_byte_cnt = 0;
      HIDMouse[2] = -HIDMouse[2]; // ��תY��
      if ( USB_Ready == TRUE && priority_USB == TRUE ) {
        tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB����¼�
      } else if ( BLE_Ready == TRUE ) {
        tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //��������¼�
      } else if ( RF_Ready == TRUE ) {
        tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF�����¼�
      }
    }
    PS2_En_Data_Report();
  }
}

/*******************************************************************************
* Function Name  : HID_VOL_Process
* Description    : �������ƴ�����
* Input          : ��
* Return         : ��
*******************************************************************************/
__attribute__((weak)) void HID_VOL_Process(void)
{
  if ( USB_Ready == TRUE && priority_USB == TRUE ) {
    tmos_set_event( usbTaskID, USB_VOL_EVENT );  //USB�����¼�
  } else if ( BLE_Ready == TRUE ) {
    tmos_set_event( hidEmuTaskId, START_VOL_REPORT_EVT );  //���������¼�
  } else if ( RF_Ready == TRUE ) {
    tmos_set_event( RFTaskId, SBP_RF_VOL_REPORT_EVT );  // RF�����¼�
  }
}

/*******************************************************************************
* Function Name  : SW_PaintedEgg_Process
* Description    : �ʵ����������
* Input          : ��
* Return         : ��
*******************************************************************************/
__attribute__((weak)) void SW_PaintedEgg_Process(void)
{
  KEYBOARD_Detection();
  if (KEYBOARD_data_ready != 0) {
    KEYBOARD_data_ready = 0;
    if (KEYBOARD_Custom_Function() != 0) {
      switch (Keyboarddat->Key1) {
        case KEY_W: BodyDir[0] = DirUp; break;
        case KEY_S: BodyDir[0] = DirDown; break;
        case KEY_A: BodyDir[0] = DirLeft; break;
        case KEY_D: BodyDir[0] = DirRight; break;
      }
    }
  }
  if (++EP_counter > 50) {  // 50���¼�����һ��
    EP_counter = 0;
    MoveSnake();
  }
}

/*******************************************************************************
* Function Name  : SW_OLED_Capslock_Process
* Description    : ��Сд״̬OLED������(���LED״̬������CapsLockָʾ)
* Input          : ��
* Return         : ��
*******************************************************************************/
__attribute__((weak)) void SW_OLED_Capslock_Process(void)
{
  if ( USB_Ready && priority_USB && CapsLock_LEDOn_state != USB_CapsLock_LEDOn ) {
    CapsLock_LEDOn_state = USB_CapsLock_LEDOn;
    OLED_ShowCapslock(56, 1, CapsLock_LEDOn_state);
  } else if ( BLE_Ready && !priority_USB && CapsLock_LEDOn_state != BLE_CapsLock_LEDOn ) {
    CapsLock_LEDOn_state = BLE_CapsLock_LEDOn;
    OLED_ShowCapslock(56, 1, CapsLock_LEDOn_state);
  } else if ( RF_Ready && !priority_USB && CapsLock_LEDOn_state != RF_CapsLock_LEDOn ) {
    CapsLock_LEDOn_state = RF_CapsLock_LEDOn;
    OLED_ShowCapslock(56, 1, CapsLock_LEDOn_state);
  }
}

/*******************************************************************************
* Function Name  : SW_OLED_UBStatus_Process
* Description    : USB��BLE״̬OLED������(���USB/����/RF����״̬������״̬�ı�OLED��ʾ)
* Input          : ��
* Return         : ��
*******************************************************************************/
__attribute__((weak)) void SW_OLED_UBStatus_Process(void)
{
  if (connection_state[0] != USB_Ready) {
    connection_state[0] = USB_Ready;
    if ( USB_Ready ) OLED_ShowString(8, 0, "USB");
    else OLED_ShowString(8, 0, "   ");
    if ( USB_Ready ^ BLE_Ready ) priority_USB = USB_Ready;
    // ͬʱ����USB������/RFʱ������ʾ
    if ( USB_Ready && (BLE_Ready || RF_Ready) ) OLED_ShowOK(26 + !priority_USB * 30, 0, TRUE);
    else {
      OLED_ShowOK(26, 0, FALSE);
      OLED_ShowOK(56, 0, FALSE);
    }
  } else if (connection_state[1] != BLE_Ready) {
    connection_state[1] = BLE_Ready;
//    HalLedSet(HAL_LED_1, BLE_Ready);
    if ( BLE_Ready ) OLED_ShowString(38, 0, "BLE");
    else {
      OLED_ShowString(38, 0, "   ");
      if ( EnterPasskey_flag == TRUE ) {  // ����������״̬
        EnterPasskey_flag = FALSE;
        BLE_Passkey = 0;
        OLED_PRINT("Close!");
        tmos_start_task( halTaskID, OLED_EVENT, MS1_TO_SYSTEM_TIME(3000) );
      }
    }
    if ( USB_Ready ^ BLE_Ready ) priority_USB = USB_Ready;
    // ͬʱ����USB������ʱ������ʾ
    if ( USB_Ready && BLE_Ready ) OLED_ShowOK(26 + !priority_USB * 30, 0, TRUE);
    else {
      OLED_ShowOK(26, 0, FALSE);
      OLED_ShowOK(56, 0, FALSE);
    }
  } else if (connection_state[2] != RF_Ready) {
    connection_state[2] = RF_Ready;
    if ( RF_Ready ) OLED_ShowString(41, 0, "RF");
    else OLED_ShowString(41, 0, "  ");
    if ( USB_Ready ^ RF_Ready ) priority_USB = USB_Ready;
    // ͬʱ����USB��RFʱ������ʾ
    if ( USB_Ready && RF_Ready ) OLED_ShowOK(26 + !priority_USB * 30, 0, TRUE);
    else {
      OLED_ShowOK(26, 0, FALSE);
      OLED_ShowOK(56, 0, FALSE);
    }
  }
}

/*******************************************************************************
* Function Name  : HW_Battery_Process
* Description    : ��ص���������
* Input          : ��
* Return         : ��
*******************************************************************************/
__attribute__((weak)) void HW_Battery_Process(void)
{
  BATTERY_ADC_Calculation( );
#if (defined HAL_OLED)  && (HAL_OLED == TRUE)
  if ( EnterPasskey_flag == FALSE ) { // ���Ƶ��
    BATTERY_DrawBMP( );
  }
  if ( BAT_chrg != BAT_IS_CHARGING ) {  // �жϳ���ź�
    BAT_chrg = BAT_IS_CHARGING;
    if ( BAT_chrg ) OLED_DrawBMP(72, 0, 81, 4, (uint8_t*)ChargeBattery);
    else OLED_Clr(72, 0, 81, 4);
  }
#endif
  BATTERY_DMA_ENABLE( );  // DMAʹ��
}

/*******************************************************************************
* Function Name  : HW_WS2812_Process
* Description    : �������⴦����
* Input          : ��
* Return         : ��
*******************************************************************************/
__attribute__((weak)) void HW_WS2812_Process(void)
{
  if (USB_Ready == TRUE) {
    WS2812_Send( );
  }
}

/*******************************************************************************
 *  ������ע��
 *******************************************************************************/
HID_ProcessFunc_s HID_ProcessFunc_v = {
  .keyboard_func = HID_KEYBOARD_Process,
  .mouse_func = HID_PS2TP_Process,
  .volume_func = HID_VOL_Process,
};
SW_ProcessFunc_s SW_ProcessFunc_v = {
  .paintedegg_func = SW_PaintedEgg_Process,
  .oled_capslock_func = SW_OLED_Capslock_Process,
  .oled_UBstatus_func = SW_OLED_UBStatus_Process,
};
HW_ProcessFunc_s HW_ProcessFunc_v = {
  .battery_func = HW_Battery_Process,
  .ws2812_func = HW_WS2812_Process,
};

/*******************************************************************************
 * @fn          Lib_Calibration_LSI
 *
 * @brief       �ڲ�32kУ׼
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void Lib_Calibration_LSI( void )
{
  Calibration_LSI( Level_128 ); // Level_64
}

#if (defined (BLE_SNV)) && (BLE_SNV == TRUE)
/*******************************************************************************
 * @fn          Lib_Read_Flash
 *
 * @brief       Lib ����Flash�ص�
 *
 * input parameters
 *
 * @param       addr.
 * @param       num.
 * @param       pBuf.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
u32 Lib_Read_Flash( u32 addr, u32 num, u32 *pBuf )
{
  EEPROM_READ( addr, pBuf, num * 4 );
  return 0;
}

/*******************************************************************************
 * @fn          Lib_Write_Flash
 *
 * @brief       Lib ����Flash�ص�
 *
 * input parameters
 *
 * @param       addr.
 * @param       num.
 * @param       pBuf.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
u32 Lib_Write_Flash( u32 addr, u32 num, u32 *pBuf )
{
  EEPROM_ERASE( addr, EEPROM_PAGE_SIZE*2 );
  EEPROM_WRITE( addr, pBuf, num * 4 );
  return 0;
}
#endif

/*******************************************************************************
 * @fn          CH57X_BLEInit
 *
 * @brief       BLE ���ʼ��
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void CH57X_BLEInit( void )
{
  uint8 i;
  bleConfig_t cfg;
  if ( tmos_memcmp( VER_LIB, VER_FILE, strlen( VER_FILE )  ) == FALSE )
  {
    PRINT( "head file error...\n" );
    while( 1 )
      ;
  }
  SysTick_Config( SysTick_LOAD_RELOAD_Msk );
  PFIC_DisableIRQ( SysTick_IRQn );

  tmos_memset( &cfg, 0, sizeof(bleConfig_t) );
  cfg.MEMAddr = ( u32 ) MEM_BUF;
  cfg.MEMLen = ( u32 ) BLE_MEMHEAP_SIZE;
  cfg.BufMaxLen = ( u32 ) BLE_BUFF_MAX_LEN;
  cfg.BufNumber = ( u32 ) BLE_BUFF_NUM;
  cfg.TxNumEvent = ( u32 ) BLE_TX_NUM_EVENT;
  cfg.TxPower = ( u32 ) BLE_TX_POWER;
#if (defined (BLE_SNV)) && (BLE_SNV == TRUE)
  FLASH_ROM_LOCK( 0 );                    // ����flash
  cfg.SNVAddr = ( u32 ) BLE_SNV_ADDR;
  cfg.readFlashCB = Lib_Read_Flash;
  cfg.writeFlashCB = Lib_Write_Flash;
#endif
#if( CLK_OSC32K )	
  cfg.SelRTCClock = ( u32 ) CLK_OSC32K;
#endif
  cfg.ConnectNumber = ( PERIPHERAL_MAX_CONNECTION & 3 ) | ( CENTRAL_MAX_CONNECTION << 2 );
  cfg.srandCB = SYS_GetSysTickCnt;
#if (defined TEM_SAMPLE)  && (TEM_SAMPLE == TRUE)
  cfg.tsCB = HAL_GetInterTempValue;    // �����¶ȱ仯У׼RF���ڲ�RC( ����7���϶� )
#if( CLK_OSC32K )
  cfg.rcCB = Lib_Calibration_LSI;    // �ڲ�32Kʱ��У׼
#endif
#endif
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
  cfg.sleepCB = CH58X_LowPower;    // ����˯��
#endif
#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
  for ( i = 0; i < 6; i++ )
    cfg.MacAddr[i] = MacAddr[5 - i];
#else
  {
    uint8 MacAddr[6];
    GetMACAddress( MacAddr );
    for(i=0;i<6;i++) cfg.MacAddr[i] = MacAddr[i];    // ʹ��оƬmac��ַ
  }
#endif
  if ( !cfg.MEMAddr || cfg.MEMLen < 4 * 1024 )
    while( 1 )
      ;
  i = BLE_LibInit( &cfg );
  if ( i )
  {
    PRINT( "LIB init error code: %x ...\n", i );
    while( 1 )
      ;
  }
}

/*******************************************************************************
 * @fn          HAL_ProcessEvent
 *
 * @brief       Ӳ����������
 *
 * input parameters
 *
 * @param       task_id.
 * @param       events.
 *
 * output parameters
 *
 * @param       events.
 *
 * @return      None.
 */
tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  uint8 * msgPtr;

  if ( events & SYS_EVENT_MSG )
  {    // ����HAL����Ϣ������tmos_msg_receive��ȡ��Ϣ��������ɺ�ɾ����Ϣ��
    msgPtr = tmos_msg_receive( task_id );
    if ( msgPtr )
    {
      /* De-allocate */
      tmos_msg_deallocate( msgPtr );
    }
    return events ^ SYS_EVENT_MSG;
  }

  // ����LED��˸�¼�
  if ( events & LED_BLINK_EVENT )
  {
#if (defined HAL_LED) && (HAL_LED == TRUE)
    HalLedUpdate( );
#endif // HAL_LED
    return events ^ LED_BLINK_EVENT;
  }

  // ���ذ����¼�
  if ( events & KEY_EVENT )
  {
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
    HAL_KeyPoll(); /* Check for keys */
    tmos_start_task( halTaskID, KEY_EVENT, MS1_TO_SYSTEM_TIME(100) );
#endif
    return events ^ KEY_EVENT;
  }

  // ���ADC����¼�
  if ( events & BATTERY_EVENT )
  {
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
    HW_ProcessFunc_v.battery_func();
#endif
    tmos_start_task( halTaskID, BATTERY_EVENT, MS1_TO_SYSTEM_TIME(5000) );  // 5s���²���ֵ
    return events ^ BATTERY_EVENT;
  }

  // ��ʱ��ѭ���¼�
  if ( events & MAIN_CIRCULATION_EVENT)
  {
    // �ʵ�ģʽ
#if (defined SW_PAINTEDEGG) && (SW_PAINTEDEGG == TRUE)
    if ( PaintedEggMode == TRUE ) {
      SW_ProcessFunc_v.paintedegg_func();
      goto main_circulation_end;
    }
#endif
    // ��괦��
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
    HID_ProcessFunc_v.mouse_func();
#endif
    // ���̴���
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
    HID_ProcessFunc_v.keyboard_func();
#endif
    // OLED��Ϣ���´���
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    SW_ProcessFunc_v.oled_UBstatus_func();
    SW_ProcessFunc_v.oled_capslock_func();
#endif
    main_circulation_end:
    tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, MS1_TO_SYSTEM_TIME(5) ); // 5ms����
    return events ^ MAIN_CIRCULATION_EVENT;
  }

  // WS2812�����¼�
  if ( events & WS2812_EVENT )
  {
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
    HW_ProcessFunc_v.ws2812_func();
#endif
    tmos_start_task( halTaskID, WS2812_EVENT, MS1_TO_SYSTEM_TIME(60) ); // 60ms���ڿ��Ʊ���
    return events ^ WS2812_EVENT;
  }

  // OLEDȡ�����¼�
  if ( events & OLED_EVENT )
  {
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    OLED_PRINT("");
#endif
    return events ^ OLED_EVENT;
  }

  // USB Ready�¼�
  if ( events & USB_READY_EVENT )
  {
    USB_Ready = TRUE;
    return events ^ USB_READY_EVENT;
  }

  // Ӳ����ʼ���¼�
  if ( events & HAL_REG_INIT_EVENT )
  {
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)	// У׼���񣬵���У׼��ʱС��10ms
    BLE_RegInit();    // У׼RF
#if( CLK_OSC32K )	
    Lib_Calibration_LSI();    // У׼�ڲ�RC
#endif
    tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );
    return events ^ HAL_REG_INIT_EVENT;
#endif
  }

  // �����¼�
  if ( events & HAL_TEST_EVENT )
  {
    PRINT( "*\n" );
    tmos_start_task( halTaskID, HAL_TEST_EVENT, MS1_TO_SYSTEM_TIME( 1000 ) );
    return events ^ HAL_TEST_EVENT;
  }

  return 0;
}

/*******************************************************************************
 * @fn          HAL_Init
 *
 * @brief       Ӳ����ʼ��
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HAL_Init()
{
  char debug_info[520] = "All Ready!";
  halTaskID = TMOS_ProcessEventRegister( HAL_ProcessEvent );
  HAL_TimeInit();
#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE)
  HAL_SleepInit();
#endif
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HAL_KeyInit( );
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
  HAL_OLED_Init( );
#endif
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
  BATTERY_Init( );
#endif
#if (defined HAL_USB) && (HAL_USB == TRUE)
  HAL_USBInit( );
#endif
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
  PS2_Init(debug_info, TRUE); // PS/2�ж�ʵ��
#endif
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  KEYBOARD_Init( );
#endif
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
  WS2812_PWM_Init( );
#endif
#if (defined HAL_RF) && (HAL_RF == TRUE)
  FLASH_Read_RForBLE( );
#endif
#if (defined HAL_LED) && (HAL_LED == TRUE)
  debug_info[7] = '\0';
  if ( strcmp(debug_info, "[ERROR]") == 0 ) {
    HAL_LedInit(1);
  } else {
    HAL_LedInit(0);
  }
#endif
#if ( defined BLE_CALIBRATION_ENABLE ) && ( BLE_CALIBRATION_ENABLE == TRUE )
  tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );    // ���У׼���񣬵���У׼��ʱС��10ms
#endif
  PRINT("%s\n", debug_info);
  /******* ��ʼ��OLED UI *******/
  OLED_ShowString(2, 1, "L1");
  OLED_ShowChar(20, 1, 'S');
  OLED_ShowNum(26, 1, FLASH_Read_LEDStyle( ), 1);
  OLED_ShowChar(38, 1, 'D');
  FLASH_Read_DeviceID();
  OLED_ShowNum(44, 1, DeviceAddress[5], 1);
  OLED_ShowCapslock(56, 1, FALSE);
  OLED_DrawBMP(91, 0, 91 + 32, 3, (uint8_t*)EmptyBattery);  // ���ƿյ��
  debug_info[7] = '\0';
  { // ��ʱ��ӡ
  OLED_PRINT("%s", debug_info);
  tmos_start_task( halTaskID, OLED_EVENT, MS1_TO_SYSTEM_TIME(3000) );
  }
//  tmos_start_task( halTaskID, HAL_TEST_EVENT, 1600 );    // ��Ӳ�������
}

/*******************************************************************************
 * @fn          HAL_GetInterTempValue
 *
 * @brief       ���ʹ����ADC�жϲ��������ڴ˺�������ʱ�����ж�.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
uint16 HAL_GetInterTempValue( void )
{
  uint8 sensor, channel, config, tkey_cfg;
  uint16 adc_data;
  
  tkey_cfg = R8_TKEY_CFG;
  sensor = R8_TEM_SENSOR;
  channel = R8_ADC_CHANNEL;
  config = R8_ADC_CFG;
  ADC_InterTSSampInit();
  R8_ADC_CONVERT |= RB_ADC_START;
  while( R8_ADC_CONVERT & RB_ADC_START )
    ;
  adc_data = R16_ADC_DATA;
  R8_TEM_SENSOR = sensor;
  R8_ADC_CHANNEL = channel;
  R8_ADC_CFG = config;
  R8_TKEY_CFG = tkey_cfg;
  return ( adc_data );
}

/******************************** endfile @ mcu ******************************/
