/********************************** (C) COPYRIGHT *******************************
 * File Name          : UART.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description        : UART驱动源文件
 *******************************************************************************/

#include "UART1.h"

//初始化串口1
void UART1_Init(void)
{
  /* 配置串口1：先配置IO口模式，再配置串口 */
  GPIOA_SetBits( GPIO_Pin_9 );
  GPIOA_ModeCfg( GPIO_Pin_8, GPIO_ModeIN_PU );         // RXD-配置上拉输入
  GPIOA_ModeCfg( GPIO_Pin_9, GPIO_ModeOut_PP_5mA );    // TXD-配置推挽输出，注意先让IO口输出高电平
  UART1_DefInit();
}

//重映射初始化串口1
void UART1_Remap_Init(void)
{
  /* 配置串口1：先配置IO口模式，再配置串口 */
  GPIOA_SetBits( GPIO_Pin_13 );
  GPIOA_ModeCfg( GPIO_Pin_12, GPIO_ModeIN_PU );         // RXD-配置上拉输入
  GPIOA_ModeCfg( GPIO_Pin_13, GPIO_ModeOut_PP_5mA );    // TXD-配置推挽输出，注意先让IO口输出高电平
  GPIOPinRemap( ENABLE, RB_PIN_UART1 );
  UART1_DefInit();
}
