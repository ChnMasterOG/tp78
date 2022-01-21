/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2021/12/06
* Description        : TP78小红点+蓝牙键盘应用主函数及任务系统初始化
*******************************************************************************/

/********************************** (C) COPYRIGHT *******************************
 *  key-board pins:   PB10 & PB11 --- USB1; [PA0 ~ PA7 & PA9 & PA11 ~ PA15] & [PB2 ~ PB7] --- KeyBoard
 *                    PB13 & PB12 --- PS/2 Mouse; PB9 & PB8 --- OLED; PA10 --- W2812(TMR1)
 *                    PA8 & PB14 --- Battery
 *  core-board pins:  PB0 --- LED; PB1 --- KEY; PB20 & PB21 --- R/TXD3_
 ********************************* (C) COPYRIGHT ********************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "CH58x_common.h"
#include "HAL.h"
#include "hiddev.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84,0xC2,0xE4,0x03,0x02,0x02};
#endif

/*******************************************************************************
* Function Name  : Main_Circulation
* Description    : 主循环
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__HIGH_CODE
void Main_Circulation()
{
  // 开启TMOS任务调度
  while(1){
    TMOS_SystemProcess( );
  }
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
#if (defined (DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
  PWR_DCDCCfg( ENABLE );
#endif
  SetSysClock( CLK_SOURCE_PLL_60MHz );
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
  GPIOB_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
#endif
#ifdef DEBUG
  GPIOB_SetBits(1<<21); // PB21
  GPIOB_ModeCfg(1<<21, GPIO_ModeOut_PP_5mA);
  GPIOPinRemap(ENABLE, RB_PIN_UART3);
  UART3_DefInit( );
#endif
  PRINT("%s\n",VER_LIB);
  CH57X_BLEInit( );
	HAL_Init( );
	GAPRole_PeripheralInit( );
	HidDev_Init( ); 
	HidEmu_Init( );
  tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, 10 ); // 主循环
  tmos_start_task( halTaskID, WS2812_EVENT, 10 );  // 背光控制
  Main_Circulation();
}

/******************************** endfile @ main ******************************/
