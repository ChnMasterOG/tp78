/********************************** (C) COPYRIGHT *******************************
 * File Name          : BATTERY.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/1/26
 * Description        : 电池ADC采样源文件
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

__attribute__((aligned(2))) UINT16 BAT_abcBuff[ADC_MAXBUFLEN];
UINT32 BAT_adcVal = 0;
BOOL BAT_chrg = FALSE;
static UINT32 BAT_adcHistory = 0;
static signed short RoughCalib_Value = 0;

/*******************************************************************************
* Function Name  : BATTERY_Init
* Description    : 电池ADC初始化
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_Init( void )
{
  uint8_t i;
//  ADC_InterTSSampInit();
//  RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到变量 RoughCalib_Value中
  // bat charging io
  GPIOB_ModeCfg( GPIO_Pin_14, GPIO_ModeIN_PU );
  // adc init
  GPIOA_ModeCfg( GPIO_Pin_8, GPIO_ModeIN_Floating );
  ADC_ExtSingleChSampInit( SampleFreq_3_2, ADC_PGA_0 );
  ADC_ChannelCfg( 12 );
  ADC_StartUp();
  R8_ADC_AUTO_CYCLE = 0;    // ADC自动转换周期：256*16/60000000s = 68us
  BATTERY_DMA_ENABLE( );
  tmos_start_task( halTaskID, BATTERY_EVENT, MS1_TO_SYSTEM_TIME(1000) );  // 等待稳定：1s
}

/*******************************************************************************
* Function Name  : BATTERY_DMA_ENABLE
* Description    : 电池ADC DMA使能, 将转换结果通过DMA载入BAT_abcBuff中
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_DMA_ENABLE( void )
{
  R16_ADC_DMA_BEG = (UINT16) (UINT32) &BAT_abcBuff[0];
  R16_ADC_DMA_END = (UINT16) (UINT32) &BAT_abcBuff[ADC_MAXBUFLEN];
  R8_ADC_CTRL_DMA = RB_ADC_DMA_ENABLE | RB_ADC_AUTO_EN | RB_ADC_CONT_EN; // 定时间隔自动连续ADC采样; ADC连续转换模式; DMA地址循环功能使能
}

/*******************************************************************************
* Function Name  : BATTERY_ADC_Convert
* Description    : 电池ADC连续转换读取电压值
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_ADC_Convert( void )
{
  uint8_t i;
  for(i = 0; i < ADC_MAXBUFLEN; i++) {
    BAT_abcBuff[i] = ADC_ExcutSingleConver();      // 连续采样ADC_MAXBUFLEN次
  }
  BATTERY_ADC_Calculation( );
//  OLED_PRINT("ADC: %d", BAT_adcVal);
}

/*******************************************************************************
* Function Name  : BATTERY_ADC_Calculation
* Description    : 计算电池ADC电压值, 保存在BAT_adcVal中
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_ADC_Calculation( void )
{
  uint8_t i;
  UINT32 BAT_adcVal_tmp = 0;
  for (i = 0; i < ADC_MAXBUFLEN; i++) {
    BAT_adcVal_tmp += BAT_abcBuff[i] + RoughCalib_Value;
  }
  BAT_adcVal_tmp /= ADC_MAXBUFLEN;
  if ( BAT_adcHistory > 4 * BAT_FLOATING_VAL + BAT_adcVal_tmp ) return;  // 突然掉电
  BAT_adcHistory = BAT_adcVal;
  BAT_adcVal = BAT_adcVal_tmp;
}

/*******************************************************************************
* Function Name  : BATTERY_ADC_GetLevel
* Description    : 获取电池ADC等级(1:0%~25%, 2:25%~50%, 3:50%~75%, 4:75%~100%, 0和5代表超出范围)
* Input          : ADC值
* Return         : 0/1/2/3/4/5
*******************************************************************************/
static UINT8 BATTERY_ADC_GetLevel( UINT32 adc_val )
{
  if ( adc_val < BAT_MINADCVAL ) return 0;
  else if ( adc_val < BAT_25PERCENT_VAL ) return 1;
  else if ( adc_val < BAT_50PERCENT_VAL ) return 2;
  else if ( adc_val < BAT_75PERCENT_VAL ) return 3;
  else if ( adc_val < BAT_MAXADCVAL ) return 4;
  else return 5;
}

/*******************************************************************************
* Function Name  : BATTERY_DrawBMP
* Description    : 绘制电池图案(仅电量等级改变才绘制)
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_DrawBMP( void )
{
  const UINT8 BMP_StartX = 91;
  UINT8 i, j;
  UINT8 BAT_level = BATTERY_ADC_GetLevel(BAT_adcVal);
  BOOL isFloating = ABS((long)BAT_adcHistory - (long)BAT_adcVal) >= BAT_FLOATING_VAL;
  if (BATTERY_ADC_GetLevel(BAT_adcHistory) != BAT_level) { // 电量等级变化
    OLED_DrawBMP(BMP_StartX, 0, BMP_StartX + 32, 3, (uint8_t*)EmptyBattery);  // 绘制空电池
    OLED_Set_Pos(BMP_StartX + 2, 0);
    for (i = 0; i < BAT_level; i++) { // 绘制电量(1)
      OLED_WR_Byte(0x10, OLED_DATA);
      for (j = 0; j < 4; j++) {
        OLED_WR_Byte(0xD0, OLED_DATA);
      }
    }
    OLED_Set_Pos(BMP_StartX + 2, 1);
    for (i = 0; i < BAT_level; i++) { // 绘制电量(2)
      OLED_WR_Byte(0x00, OLED_DATA);
      for (j = 0; j < 4; j++) {
        OLED_WR_Byte(0xFF, OLED_DATA);
      }
    }
    OLED_Set_Pos(BMP_StartX + 2, 2);
    for (i = 0; i < BAT_level; i++) { // 绘制电量(下半部分)
      OLED_WR_Byte(0x08, OLED_DATA);
      for (j = 0; j < 4; j++) {
        OLED_WR_Byte(0x0B, OLED_DATA);
      }
    }
  }
  // 无论电量等级是否变化都给出电量是否浮动
  if ( isFloating ) {
    OLED_Set_Pos(BMP_StartX + 29, 0);
    OLED_WR_Byte(0xE1, OLED_DATA);
    OLED_WR_Byte(0x0B, OLED_DATA);
  } else {
    OLED_Set_Pos(BMP_StartX + 29, 0);
    OLED_WR_Byte(0xE0, OLED_DATA);
    OLED_WR_Byte(0x00, OLED_DATA);
  }
  // 无论电量等级是否变化都输出ADC值
  OLED_Set_Pos(BMP_StartX + 4, 3);
  OLED_ShowNum(BMP_StartX + 4, 3, BAT_adcVal, 4);
}
