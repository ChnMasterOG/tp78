/********************************** (C) COPYRIGHT *******************************
* File Name          : BLE.h
* Author             : ChnMasterOG
* Version            : V1.2
* Date               : 2022/1/27
* Description        : 
*******************************************************************************/

#ifndef BLE_H
#define BLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"

/*********************************************************************
 * CONSTANTS
 */

// Task Events
#define START_DEVICE_EVT                              0x0001
#define TEST_REPORT_EVT                               0x0002
#define START_PARAM_UPDATE_EVT                        0x0004
#define START_PHY_UPDATE_EVT                          0x0008
#define CHANGE_ADDR_EVT                               0x0010
#define START_ENTER_PASSKEY_EVT                       0x0020
#define START_SEND_PASSKEY_EVT                        0x0040
#define START_MOUSE_REPORT_EVT                        0x1000
#define START_KEYBOARD_REPORT_EVT                     0x2000
#define START_VOL_REPORT_EVT                          0x4000
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */
  
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Read BLE device ID from Flash
 */
extern void FLASH_Read_DeviceID( void );

/*
 * Write BLE device ID to Flash
 */
extern void FLASH_Write_DeviceID( uint8_t DeviceID );

/*
 * Task Initialization for the BLE Application
 */
extern void HidEmu_Init( void );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 HidEmu_ProcessEvent( uint8 task_id, uint16 events );

extern tmosTaskID hidEmuTaskId;
extern BOOL BLE_Ready;
extern uint8_t BLE_SelectHostIndex;
extern uint8_t DeviceAddress[6];
extern BOOL EnterPasskey_flag;
extern uint32_t BLE_Passkey;

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif 
