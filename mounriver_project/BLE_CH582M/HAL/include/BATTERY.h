/********************************** (C) COPYRIGHT *******************************
* File Name          : BATTERY.h
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2021/12/15
* Description        :
*******************************************************************************/

#ifndef BATTERY_H
#define BATTERY_H

  #define ADC_MAXBUFLEN   10

  void BATTERY_Init( void );
  void BATTERY_DMA_ENABLE( void );
  void BATTERY_ADC_Calculation( void );

  extern UINT16 BAT_abcBuff[ADC_MAXBUFLEN];
  extern UINT32 BAT_adcVal;

#endif
