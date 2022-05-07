/********************************** (C) COPYRIGHT *******************************
* File Name          : BATTERY.h
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2021/12/15
* Description        :
*******************************************************************************/

#ifndef BATTERY_H
#define BATTERY_H

  #define ADC_MAXBUFLEN       10

  #define BAT_MINADCVAL       3413  // 3.5V/2
  #define BAT_MAXADCVAL       3998  // 4.1V/2

  #define BAT_25PERCENT_VAL   3559  // 25%
  #define BAT_50PERCENT_VAL   3705  // 50%
  #define BAT_75PERCENT_VAL   3851  // 75%

  #define BAT_FLOATING_VAL    50    // ADC浮动超过该值则警告

  #define BAT_IS_CHARGING     !(R32_PB_PIN & GPIO_Pin_14)

  void BATTERY_Init( void );
  void BATTERY_DMA_ENABLE( void );
  void BATTERY_ADC_Convert( void );
  void BATTERY_ADC_Calculation( void );
  void BATTERY_DrawBMP( void );

  extern UINT16 BAT_abcBuff[ADC_MAXBUFLEN];
  extern UINT32 BAT_adcVal;
  extern BOOL BAT_chrg;

#endif
