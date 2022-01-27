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
#define DISCONNECT_EVT                                0x0010
#define START_MOUSE_REPORT_EVT                        0x1000
#define START_KEYBOARD_REPORT_EVT                     0x2000
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
 * Task Initialization for the BLE Application
 */
extern void HidEmu_Init( void );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 HidEmu_ProcessEvent( uint8 task_id, uint16 events );

/*
 * Save current host address array to flash
 */
uint8_t hidEmu_SaveHostAddr( uint8_t index );

extern tmosTaskID hidEmuTaskId;
extern BOOL BLE_Ready;
extern uint8_t BLE_HostAddr[BLE_DEVICE_NUM+1][B_ADDR_LEN];
extern uint8_t BLE_SelectHostIndex;

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif 
