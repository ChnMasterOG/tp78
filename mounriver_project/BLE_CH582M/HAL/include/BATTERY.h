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

  #define BAT_MINADCVAL       3087
  #define BAT_MAXADCVAL       3813  // 0.43*Vbat, Vref=0.97V

  #define BAT_25PERCENT_VAL   3269  // 25%
  #define BAT_50PERCENT_VAL   3450  // 50%
  #define BAT_75PERCENT_VAL   3632  // 75%

  void BATTERY_Init( void );
  void BATTERY_DMA_ENABLE( void );
  void BATTERY_ADC_Convert( void );
  void BATTERY_ADC_Calculation( void );
  void BATTERY_DrawBMP( void );

  extern UINT16 BAT_abcBuff[ADC_MAXBUFLEN];
  extern UINT32 BAT_adcVal;

#endif
