/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/11/15
 * Description 		  : test
 *******************************************************************************/

#include "CH58x_common.h"
#include "PS2Driver.h"
#include "UART1.h"

extern Mousestate PS2dat;
char buf[520];
uint8_t PS2_bit_cnt = 0,
        PS2_byte_cnt = 0,
        PS2_byte = 0,
        PS2_high_cnt = 0,
        PS2_data_ready = 0;

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

  UART1_Init();
  printf("hello\n");

  if (PS2_Init(buf, 1) != 0) {
      printf("%s\n", buf);
      while (1);
  }

  printf("ready\n");

  while (1) {
      //中断
      if (PS2_data_ready != 0) {
          PS2_data_ready = 0;
          if (PS2_byte_cnt == 3) {
              PS2_byte_cnt = 0;
//              printf("%d %d %d\n", PS2dat.data[0], PS2dat.data[1], PS2dat.data[2]);
              printf("%d %d %d %d\n", PS2dat.LeftBtn, PS2dat.RightBtn, PS2dat.XMovement, PS2dat.YMovement);
          }
          PS2_En_Data_Report();
      }
      DelayUs(50);
      //非中断
//      PS2_ReadMouseData(&PS2dat);
//      printf("%d %d %d %d\n", PS2dat.LeftBtn, PS2dat.RightBtn, PS2dat.XMovement, PS2dat.YMovement);
  }

}

__INTERRUPT
__HIGH_CODE
void GPIOA_IRQHandler( void )
{
    if ( GPIOA_ReadITFlagBit( GPIO_Pin_4 ) != 0 ) {
        GPIOA_ClearITFlagBit( GPIO_Pin_4 );
        GPIOB_InverseBits( GPIO_Pin_0 );
    }
}

__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler( void )
{
    if ( PS2CLK_GPIO_(ReadITFlagBit)( PS2CLK_Pin ) != 0 ) {
        PS2CLK_GPIO_(ClearITFlagBit)( PS2CLK_Pin );
        ++PS2_bit_cnt;
        if (PS2_bit_cnt == 1) { //起始位
            PS2_byte = 0;
            PS2_high_cnt = 0;
        }
        else if (PS2_bit_cnt <= 9) {
            PS2_byte >>= 1;
            if (PS2DATA_State()) {
                  PS2_byte |= 0x80;
                ++PS2_high_cnt;
            }
        }
        else if (PS2_bit_cnt == 10) {   //校验位
//            if ((PS2_high_cnt & 1) == (PS2DATA_State() != 0)) {
//                PS2_data_ready = 2;
//            }
        }
        else if (PS2_bit_cnt == 11) {   //停止位
            PS2dat.data[PS2_byte_cnt++] = PS2_byte;
            PS2_Dis_Data_Report();
            PS2_bit_cnt = 0;
            PS2_data_ready = 1;
        }
    }
}
