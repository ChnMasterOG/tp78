/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : TP87
 * Version            : V1.1
 * Date               : 2021/11/27
 * Description 		  : USB键盘测试
 *******************************************************************************/

/********************************** (C) COPYRIGHT *******************************
 *  M.2引脚分配：    PB10 & PB11 --- USB1; PA0 ~ PA7 & PA10 ~ PA15 & PB2 ~ PB7 --- KeyBoard
 *                PB13 & PB12 --- PS/2 Mouse; PB9 & PB8 --- OLED; PB14 --- W2812
 *                PA8 & PA9 --- UART1(download)
 *  核心板引脚：       PB0 --- LED; PB1 --- KEY
 ********************************* (C) COPYRIGHT ********************************/

#include "CH58x_common.h"
#include "PS2Driver.h"
#include "UART1.h"
#include "USBDriver.h"
#include "OLEDDriver.h"
#include "KeyboardDriver.h"

char buf[128];

void Test_Circulation()
{
  uint8_t flag = 0;
  while (1) {
//      if (GPIOB_ReadPortPin( GPIO_Pin_1 ) != 0) {
//          DelayMs(10);
//          if (GPIOB_ReadPortPin( GPIO_Pin_1 ) == 0) continue;
//          if (flag == 0) {
//              GPIOB_InverseBits( GPIO_Pin_0 );
//              flag = 1;
//          }
//      } else flag = 0;
      HIDKey[2] = KEY_A;
      memcpy(pEP1_IN_DataBuf, HIDKey, 8);
      DevEP1_IN_Deal( 8 );
      DelayMs(5);
      HIDKey[2] = 0;
      memcpy(pEP1_IN_DataBuf, HIDKey, 8);
      DevEP1_IN_Deal( 8 );
      DelayMs(2000);
  }
}

int main()
{
  Mousestate myData;
  uint8_t res;

  SetSysClock( CLK_SOURCE_PLL_60MHz );
  RB_CLK_OSC32K_XT = 0; // 使用内部32k

  //mini board led
  GPIOB_SetBits( GPIO_Pin_0 );
  GPIOB_ModeCfg( GPIO_Pin_0, GPIO_ModeOut_PP_5mA );

  //mini board key
  GPIOB_SetBits( GPIO_Pin_1 );
  GPIOB_ModeCfg( GPIO_Pin_1, GPIO_ModeIN_PD );

  GPIOB_ResetBits( GPIO_Pin_0 );

  //uart1
  UART1_Init();
  printf("hello\n");

  //oled
  OLED_Init();

  //Keyboard
  KEY_Init();

  //PS/2
  if (PS2_Init(buf, 1) != 0) {
      printf("%s\n", buf);
      while (1);
  }
  else {
      printf("mouse ready\n");
  }

  GPIOB_SetBits( GPIO_Pin_0 );

  //USB
  pEP0_RAM_Addr = EP0_Databuf;
  pEP1_RAM_Addr = EP1_Databuf;
  pEP2_RAM_Addr = EP2_Databuf;
  pEP3_RAM_Addr = EP3_Databuf;
  USB_DeviceInit();
  PFIC_EnableIRQ( USB_IRQn );

  OLED_ShowString(0, 2, "All ready!");

  while (1) {
      if (PS2_data_ready != 0) {    //USB发送小红点鼠标数据
          PS2_data_ready = 0;
          if (PS2_byte_cnt == 3) {
              PS2_byte_cnt = 0;
              memcpy(pEP2_IN_DataBuf, PS2dat, 4);
              DevEP2_IN_Deal( 4 );
          }
          PS2_En_Data_Report();
      }
      KEY_detection();
      if (KEY_data_ready != 0) {    //USB发送键盘数据
          KEY_data_ready = 0;
          memcpy(pEP1_IN_DataBuf, Keyboarddat, 8);
          DevEP1_IN_Deal( 8 );
      }
      DelayMs(10);
  }

}

__INTERRUPT
__HIGH_CODE
void GPIOA_IRQHandler( void )   //GPIOA外部中断
{

}

__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler( void )   //GPIOB外部中断
{
    PS2_IT_handler();
}

__INTERRUPT
__HIGH_CODE
void TMR0_IRQHandler( void )    //TMR0定时中断
{

}

__INTERRUPT
__HIGH_CODE
void UART1_IRQHandler( void )   //UART1串口中断
{

}

__INTERRUPT
__HIGH_CODE
void USB_IRQHandler( void )     //USB中断服务程序,使用寄存器组1
{
    USB_DevTransProcess();
}
