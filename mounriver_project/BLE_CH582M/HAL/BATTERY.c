/********************************** (C) COPYRIGHT *******************************
 * File Name          : BATTERY.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/12/15
 * Description        : 电池ADC采样源文件
 *******************************************************************************/

#include "HAL.h"

__attribute__((aligned(2))) UINT16 BAT_abcBuff[ADC_MAXBUFLEN];
UINT32 BAT_adcVal = 0;
static signed short RoughCalib_Value = 0;

void BATTERY_Init( void )
{
  uint8_t i;
  RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到变量 RoughCalib_Value中
  // adc init
  GPIOA_ModeCfg( GPIO_Pin_8, GPIO_ModeIN_Floating );
  ADC_ExtSingleChSampInit( SampleFreq_3_2, ADC_PGA_0 );
  ADC_ChannelCfg( 12 );
  ADC_StartUp();
  R8_ADC_AUTO_CYCLE = 0;    // ADC自动转换周期：256*16/60000000s = 68us
  BATTERY_DMA_ENABLE( );
  tmos_start_task( halTaskID, BATTERY_EVENT, MS1_TO_SYSTEM_TIME(1000) );  // 等待稳定：1s
}

void BATTERY_DMA_ENABLE( void )
{
  R16_ADC_DMA_BEG = (UINT16) (UINT32) &BAT_abcBuff[0];
  R16_ADC_DMA_END = (UINT16) (UINT32) &BAT_abcBuff[ADC_MAXBUFLEN];
  R8_ADC_CTRL_DMA = RB_ADC_DMA_ENABLE | RB_ADC_AUTO_EN | RB_ADC_CONT_EN; // 定时间隔自动连续ADC采样; ADC连续转换模式; DMA地址循环功能使能
}

void BATTERY_ADC_Calculation( void )
{
  uint8_t i;
  UINT32 BAT_adcVal_tmp = 0;
  for (i = 0; i < ADC_MAXBUFLEN; i++) {
    BAT_adcVal_tmp += BAT_abcBuff[i];
  }
  BAT_adcVal_tmp /= ADC_MAXBUFLEN;
//  BAT_adcVal_tmp = BAT_adcVal_tmp + RoughCalib_Value;
  BAT_adcVal = BAT_adcVal_tmp;
}
