/********************************** (C) COPYRIGHT *******************************
 * File Name          : USBDriver.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description        : USB驱动头文件
 *******************************************************************************/

#ifndef __USBDRIVER_H
    #define __USBDRIVER_H

    #include "CH58x_common.h"

    #define DevEP0SIZE      0x40
    #define NumsLockLED     0x01
    #define CapsLockLED     0x02
    #define ScrollLockLED   0x04

    extern const UINT8 MyDevDescr[];
    extern const UINT8 MyCfgDescr[];
    extern const UINT8 MyLangDescr[];
    extern const UINT8 MyManuInfo[];
    extern const UINT8 MyProdInfo[] ;
    extern const UINT8 KeyRepDesc[];
    extern const UINT8 MouseRepDesc[];
    extern UINT8 DevConfig, USB_Ready;
    extern UINT8 SetupReqCode;
    extern UINT16 SetupReqLen;
    extern const UINT8 *pDescr;
    extern UINT8 HIDMouse[4];
    extern UINT8 HIDKey[8];
    extern UINT8 EP0_Databuf[64 + 64 + 64];   //ep0(64)+ep4_out(64)+ep4_in(64)
    extern UINT8 EP1_Databuf[64 + 64];        //ep1_out(64)+ep1_in(64)
    extern UINT8 EP2_Databuf[64 + 64];        //ep2_out(64)+ep2_in(64)
    extern UINT8 EP3_Databuf[64 + 64];        //ep3_out(64)+ep3_in(64)

    void USB_DevTransProcess( void );

#endif
