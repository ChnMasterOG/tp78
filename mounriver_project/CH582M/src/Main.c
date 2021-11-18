/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : TP87
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description 		  : USB键盘测试
 *******************************************************************************/

#include "CH58x_common.h"
#include "PS2Driver.h"
#include "UART1.h"
#include "USBDriver.h"

char buf[520];

int main()
{
  Mousestate myData;
  uint8_t res;

  SetSysClock( CLK_SOURCE_PLL_60MHz );

  //led
  GPIOB_SetBits( GPIO_Pin_0 );
  GPIOB_ModeCfg( GPIO_Pin_0, GPIO_ModeOut_PP_5mA );

  //led off
  GPIOB_ResetBits( GPIO_Pin_0 );

  //uart1
  UART1_Init();
  printf("hello\n");

  //PS/2
  if (PS2_Init(buf, 1) != 0) {
      printf("%s\n", buf);
      while (1);
  } else printf("mouse ready\n");

  //USB
  pEP0_RAM_Addr = EP0_Databuf;
  pEP1_RAM_Addr = EP1_Databuf;
  pEP2_RAM_Addr = EP2_Databuf;
  pEP3_RAM_Addr = EP3_Databuf;
  USB_DeviceInit();
  PFIC_EnableIRQ( USB_IRQn );

  while (1) {
      if (PS2_data_ready != 0) {
          PS2_data_ready = 0;
          if (PS2_byte_cnt == 3) {
              PS2_byte_cnt = 0;
              memcpy(pEP2_IN_DataBuf, &PS2dat, 4);
              DevEP2_IN_Deal( 4 );
//              printf("%d %d %d %d\n", PS2dat.LeftBtn, PS2dat.RightBtn, PS2dat.XMovement, PS2dat.YMovement);
          }
          PS2_En_Data_Report();
      }
      DelayUs(50);
  }

}

__INTERRUPT
__HIGH_CODE
void GPIOA_IRQHandler( void )
{
}

__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler( void )
{
    PS2_IT_handler();
}

__INTERRUPT
__HIGH_CODE
void USB_IRQHandler( void ) /* USB中断服务程序,使用寄存器组1 */
{
    USB_DevTransProcess();
}
