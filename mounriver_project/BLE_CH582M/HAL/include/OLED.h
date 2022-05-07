/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/21
 * Description        : OLED 9.1寸 I2C驱动头文件
 *******************************************************************************/
	
#ifndef __OLEDDRIVER_H
    #define __OLEDDRIVER_H

    #include "OLEDFONT.h"
    #include "CH58x_common.h"
    
    #define SIZE        8
    #define XLevelL		0x00
    #define XLevelH		0x10
    #define Max_Column	128
    #define Brightness	0xFF

    #define OLED_CLK_Pin        GPIO_Pin_9
    #define OLED_SDA_Pin        GPIO_Pin_8
    #define OLED_CLK_GPIO_(x)   GPIOB_ ## x
    #define OLED_SDA_GPIO_(x)   GPIOB_ ## x

    #define OLED_IIC_SCLK_Clr() OLED_CLK_GPIO_(ResetBits)(OLED_CLK_Pin)
    #define OLED_IIC_SCLK_Set() OLED_CLK_GPIO_(SetBits)(OLED_CLK_Pin)

    #define OLED_IIC_SDIN_Clr() OLED_SDA_GPIO_(ResetBits)(OLED_SDA_Pin)
    #define OLED_IIC_SDIN_Set() OLED_SDA_GPIO_(SetBits)(OLED_SDA_Pin)

    #define OLED_CMD  0	//写命令
    #define OLED_DATA 1	//写数据
    #define IIC_DELAY { OLED_IIC_Delay(); }

    #define OLED_HIS_LEN    10    //保存OLED打印历史条数
    #define OLED_HIS_DLEN   64    //每条OLED打印历史长度

    //OLED控制函数
    void OLED_IIC_Delay(void);
    void OLED_IIC_Start(void);
    void OLED_IIC_Stop(void);
    void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
    void OLED_IIC_SendByte(uint8_t Data);
    void OLED_Set_Pos(uint8_t x, uint8_t y);
    void OLED_Display_On(void);
    void OLED_Display_Off(void);
    void OLED_Clear(void);
    void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
    uint32_t oled_pow(uint8_t m,uint8_t n);
    void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
    void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr);
    void OLED_TP78Info(uint8_t *chr);
    void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);
    void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* BMP);
    void OLED_Clr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    void OLED_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    void OLED_ShowOK(uint8_t x, uint8_t y, uint8_t s);
    void OLED_ShowCapslock(uint8_t x, uint8_t y, uint8_t s);
    int OLED_printf(uint8_t x, uint8_t y, char *pFormat, ...);
    uint8_t OLED_Midx(uint8_t length, uint8_t xstart, uint8_t xend);
    void HAL_OLED_Init(void);

    extern uint8_t OLED_printf_history[OLED_HIS_LEN][OLED_HIS_DLEN+1];
#endif
	 
/*********************************************END OF FILE**********************/
