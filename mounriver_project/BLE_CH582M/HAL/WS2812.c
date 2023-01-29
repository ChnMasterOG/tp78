/********************************** (C) COPYRIGHT *******************************
 * File Name          : WS2812.c
 * Author             : ChnMasterOG
 * Version            : V1.2
 * Date               : 2022/4/13
 * Description        : WS2812����Դ����
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

__attribute__((aligned(4))) UINT32 LED_DMA_Buffer[LED_Number*24 + 42] = { TIMING_RESET };  // LED��PWM���巭ת����ֵ��������RESETʱ��Ϊ42*1.25us
UINT8 LED_BYTE_Buffer[LED_Number][3] = { 0 };
WS2812_Style_Func led_style_func = WS2812_Style_Off;  // Ĭ�ϱ��⺯��
static uint8_t style_dir = 0;
static uint32_t style_cnt = 0;

/*******************************************************************************
* Function Name  : FLASH_Read_LEDStyle
* Description    : ��Flash��ȡLED��ʽ
* Input          : None
* Return         : LED_Style_Number
*******************************************************************************/
uint8_t FLASH_Read_LEDStyle( void )
{
  uint8_t LED_Style_Number;
  EEPROM_READ( FLASH_ADDR_LEDStyle, &LED_Style_Number, 1 );
  LED_Change_flag = 1;
  switch (LED_Style_Number)
  {
    case 0:
      led_style_func = WS2812_Style_Off;
      break;
    case 1:
      led_style_func = WS2812_Style_Breath;
      break;
    case 2:
      led_style_func = WS2812_Style_Waterful;
      break;
    case 3:
      led_style_func = WS2812_Style_Touch;
      break;
    case 4:
      led_style_func = WS2812_Style_Rainbow;
      break;
    default:
      led_style_func = WS2812_Style_Off;
      LED_Style_Number = 0;
      break;
  }
  return LED_Style_Number;
}

/*******************************************************************************
* Function Name  : FLASH_Write_LEDStyle
* Description    : ��LED��ʽд��Flash
* Input          : LED_Style_Number
* Return         : None
*******************************************************************************/
void FLASH_Write_LEDStyle( uint8_t LED_Style_Number )
{
  uint8_t check;
  do {
    EEPROM_WRITE( FLASH_ADDR_LEDStyle, &LED_Style_Number, 1 );
    EEPROM_READ( FLASH_ADDR_LEDStyle, &check, 1 );
  } while (check != LED_Style_Number);
}

/*******************************************************************************
* Function Name  : WS2812_PWM_Init
* Description    : ��ʼ��PWM
* Input          : None
* Return         : None
*******************************************************************************/
void WS2812_PWM_Init( void )
{
#ifdef USE_PWM1
  // ��ʼ�� TMR1 PA10 �������PWM
  WS2812_GPIO_(SetBits)( WS2812_Pin );
  WS2812_GPIO_(ModeCfg)( WS2812_Pin, GPIO_ModeOut_PP_5mA );
  WS2812_GPIO_(ResetBits)( WS2812_Pin );

  TMR1_PWMInit( High_Level, PWM_Times_1 );
  TMR1_PWMCycleCfg( 75 );        // ���� 1.25us

  WS2812_Style_Off( );

#else
  // ��ʼ�� PB14 ���PW''M
  WS2812_GPIO_(ModeCfg)( WS2812_Pin, GPIO_ModeOut_PP_5mA );    // PB14 - PWM10
  PWMX_CLKCfg( 1 );                   // cycle = 1/Fsys = 1/60us
  PWMX_CycleCfg( PWMX_Cycle_64 );     // ����T = 64*cycle = 1.067us ~ 1.25us +- 600ns
  PWMX_ACTOUT( CH_PWM10, 0, Low_Level, ENABLE );     // 0% ռ�ձ�
#endif
}

/*******************************************************************************
* Function Name  : WS2812_Style_Off
* Description    : �ر�WS2812
* Input          : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Off( void )
{
  uint16_t i;
  for (i = 0; i < LED_Number*24; i++) LED_DMA_Buffer[i] = TIMING_ZERO;
}

/*******************************************************************************
* Function Name  : WS2812_Style_Breath
* Description    : PWM����WS2812�����Ʊ仯����
* Input          : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Breath( void )
{
  uint16_t i, j, memaddr = 0;
  for (i = 0; i < LED_Number; i++)
  {
    /* transfer data */
    for (j = 0; j < 8; j++) // GREEN data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    /* change LED state */
    if (style_dir == 0) {  // �𽥱��� - GRB��������
      ++LED_BYTE_Buffer[i][GREEN_INDEX];
      ++LED_BYTE_Buffer[i][RED_INDEX];
      ++LED_BYTE_Buffer[i][BLUE_INDEX];
    }
    else {  // �𽥱䰵 - GRB�����ݼ�
      --LED_BYTE_Buffer[i][GREEN_INDEX];
      --LED_BYTE_Buffer[i][RED_INDEX];
      --LED_BYTE_Buffer[i][BLUE_INDEX];
    }
  }
  if (LED_BYTE_Buffer[0][GREEN_INDEX] == LED_BRIGHTNESS || LED_BYTE_Buffer[0][GREEN_INDEX] == 0 ) {
    style_dir = !style_dir;
  }
}

/*******************************************************************************
* Function Name  : WS2812_Style_Waterful
* Description    : PWM����WS2812��ˮ�Ʊ仯����
* Input          : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Waterful( void )
{
  uint16_t j;
  uint32_t slow_cnt;
  if (style_cnt % Waterful_Repeat_Times != 0) {  // ��������*Waterful_Repeat_Times = ��ˮ������
    ++style_cnt;
    if (style_cnt >= LED_Number * 3 * Waterful_Repeat_Times ) { // GRB�����л� + 120ms�ƶ�һ����
      style_cnt = 0;
    }
    return;
  } else {
    slow_cnt = style_cnt / Waterful_Repeat_Times;
  }

  // �ر���һ����
  uint32_t last_cnt = slow_cnt == 0 ? LED_Number-1 : slow_cnt-1;
  for (j = 0; j < 24; j++) {
    LED_DMA_Buffer[(last_cnt % LED_Number) * 24 + j] = TIMING_ZERO;
  }
  // ������һ����
  for (j = 0; j < 24; j++) {
    if ( j >= slow_cnt / LED_Number * 8 + 4 && j < slow_cnt / LED_Number * 8 + 8 ) {
      LED_DMA_Buffer[(slow_cnt % LED_Number) * 24 + j] = TIMING_ONE;
    } else {
      LED_DMA_Buffer[(slow_cnt % LED_Number) * 24 + j] = TIMING_ZERO;
    }
  }
  ++style_cnt;
  if (style_cnt >= LED_Number * 3 * Waterful_Repeat_Times ) { // GRB�����л� + 120ms�ƶ�һ����
    style_cnt = 0;
  }
}

/*******************************************************************************
* Function Name  : WS2812_Style_Touch
* Description    : PWM����WS2812���غ����Ʊ仯����
* Input          : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Touch( void )
{
  uint16_t i, j, memaddr = 0;
  for (i = 0; i < LED_Number; i++)
  {
    /* transfer data */
    for (j = 0; j < 8; j++) // GREEN data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    /* change LED state */
    if (LED_BYTE_Buffer[i][GREEN_INDEX] > 0) --LED_BYTE_Buffer[i][GREEN_INDEX];
    if (LED_BYTE_Buffer[i][RED_INDEX] > 0) --LED_BYTE_Buffer[i][RED_INDEX];
    if (LED_BYTE_Buffer[i][BLUE_INDEX] > 0) --LED_BYTE_Buffer[i][BLUE_INDEX];
  }
}

/*******************************************************************************
* Function Name  : WS2812_Style_Rainbow
* Description    : PWM����WS2812�ʺ�Ʊ仯����
* Input          : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Rainbow( void )
{
  signed int i;
  uint16_t j, memaddr = 0;
  for (i = 0; i < LED_Number; i++)
  {
    /* transfer data */
    for (j = 0; j < 8; j++) // GREEN data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    /* �������̲������޸Ĵ˴� */
    if (i >= 0 && i <= 7) { // sixth row - RED - 8 LED
      LED_BYTE_Buffer[i][GREEN_INDEX] = 0;
      LED_BYTE_Buffer[i][RED_INDEX] = 128 - ABS(i-0-(signed int)style_cnt*7/42)*17;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 0;
    } else if (i >= 8 && i <= 19) { // fifth row - CYAN - 12 LED
      LED_BYTE_Buffer[i][GREEN_INDEX] = 128 - ABS(i-8-(signed int)style_cnt*11/42)*11;
      LED_BYTE_Buffer[i][RED_INDEX] = 0;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 128 - ABS(i-8-(signed int)style_cnt*11/42)*11;
    } else if (i >= 20 && i <= 32) { // fourth row - YELLOW - 13 LED
      LED_BYTE_Buffer[i][GREEN_INDEX] = 128 - ABS(i-20-(signed int)style_cnt*12/42)*10;
      LED_BYTE_Buffer[i][RED_INDEX] = 128 - ABS(i-20-(signed int)style_cnt*12/42)*10;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 0;
    } else if (i >= 33 && i <= 46) { // third row - PURPLE - 14 LED
      LED_BYTE_Buffer[i][GREEN_INDEX] = 0;
      LED_BYTE_Buffer[i][RED_INDEX] = 128 - ABS(i-33-(signed int)style_cnt*13/42)*9;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 128 - ABS(i-33-(signed int)style_cnt*13/42)*9;
    } else if (i >= 47 && i <= 60) { // second row - BLUE - 14 LED
      LED_BYTE_Buffer[i][GREEN_INDEX] = 0;
      LED_BYTE_Buffer[i][RED_INDEX] = 0;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 128 - ABS(i-47-(signed int)style_cnt*13/42)*9;
    } else if (i >= 61 && i <= 74) { // first row - GREEN - 14 LED
      LED_BYTE_Buffer[i][GREEN_INDEX] = 128 - ABS(i-61-(signed int)style_cnt*13/42)*9;
      LED_BYTE_Buffer[i][RED_INDEX] = 0;
      LED_BYTE_Buffer[i][BLUE_INDEX] = 0;
    }
  }
  if (style_dir == 0) { // ��������
    ++style_cnt;
  } else {  // ��������
    --style_cnt;
  }
  if (style_cnt == 42 || style_cnt == 0) style_dir = !style_dir;
}

/*******************************************************************************
* Function Name  : WS2812_Style_Custom
* Description    : PWM����WS2812�Զ���仯����(�Ƚ�RGBͼ��д��LED_BYTE_Buffer)
* Input          : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Custom( void )
{
  uint16_t i, j, memaddr = 0;
  /* transfer data */
  for (i = 0; i < LED_Number; i++)
  {
    for (j = 0; j < 8; j++) // GREEN data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][GREEN_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][RED_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
      LED_DMA_Buffer[memaddr] = ((LED_BYTE_Buffer[i][BLUE_INDEX]<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
      memaddr++;
    }
  }
}

/*******************************************************************************
* Function Name  : WS2812����RGB����
* Description    : DMA+PWM����WS2812
* Input          : None
* Return         : None
*******************************************************************************/
void WS2812_Send( void )
{
  uint16_t i;

  if ( LED_Change_flag != 0 ) {
    LED_Change_flag = 0;
    for (i = 0; i < LED_Number; i++) {  // memory set zero
      LED_BYTE_Buffer[i][0] = LED_BYTE_Buffer[i][1] = LED_BYTE_Buffer[i][2] = 0;
    }
    style_dir = style_cnt = 0;
    WS2812_Style_Off( );
  } else {
    led_style_func( ); // ���ñ仯����
  }

  TMR1_Disable();
  TMR1_PWMActDataWidth(0);
  TMR1_Enable();  // CH582M BUG
  TMR1_DMACfg( ENABLE, (UINT16) (UINT32) LED_DMA_Buffer, (UINT16) (UINT32) (LED_DMA_Buffer + LED_Number*24 + 42), Mode_Single );  // ����DMAת�������ڴ浽����
}

