/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/3/20
 * Description        : OLED 9.1寸 I2C驱动源文件
 *******************************************************************************/

#include <stdarg.h>
#include "OLED.h"

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

uint8_t OLED_printf_history[OLED_HIS_LEN][OLED_HIS_DLEN+1] = {};  // 存放OLED_printf的历史记录
static uint8_t OLED_printf_history_idx = 0;  // 存放OLED打印历史的下标

/* OLED I2C 延迟函数 */
void OLED_IIC_Delay(void)
{
	uint32_t i = 5;
	while( i-- );
}

/* OLED I2C 启动传输 */
void OLED_IIC_Start(void)
{
	OLED_IIC_SDIN_Set();
	IIC_DELAY;

	OLED_IIC_SCLK_Set() ;
	IIC_DELAY;

	OLED_IIC_SDIN_Clr();
	IIC_DELAY;

	OLED_IIC_SCLK_Clr();
	IIC_DELAY;
}

/* OLED I2C 停止传输 */
void OLED_IIC_Stop(void)
{
	OLED_IIC_SDIN_Clr();
	IIC_DELAY;

	OLED_IIC_SCLK_Set();
	IIC_DELAY;

	OLED_IIC_SDIN_Set();
	IIC_DELAY;
}

void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{
    OLED_IIC_Start();
	OLED_IIC_SendByte(0x78);
	if(cmd == 0) {
	    OLED_IIC_SendByte(0x00);
	}
	else {
		OLED_IIC_SendByte(0x40);
	}
   OLED_IIC_SendByte(dat);    
   OLED_IIC_Stop(); 
}

void OLED_IIC_SendByte(uint8_t Data)
{
    uint8_t i;
	OLED_IIC_SCLK_Clr();
	for(i = 0; i < 8; i++)
	{  
		if(Data & 0x80)	{
			OLED_IIC_SDIN_Set();
		}
		else {
			OLED_IIC_SDIN_Clr();
		} 
		Data <<= 1;
		IIC_DELAY;
		OLED_IIC_SCLK_Set();
		IIC_DELAY;
		OLED_IIC_SCLK_Clr();
		IIC_DELAY;		
	}
	OLED_IIC_SDIN_Set();
	IIC_DELAY;
	OLED_IIC_SCLK_Set();
	IIC_DELAY;   
	OLED_IIC_SCLK_Clr(); 
}

/**
  * @brief  设定OLED显示字符的位置
  * @param  uint8_t x, uint8_t y(x为列坐标，取值0~127；y为页坐标，取值0~7)
  * @retval 无
  ******************************************************************************
  * @attention
  * SSD1306命令：
  * B0~B7 1 0 1 1  0 X2 X1 X0 设置页地址 X[2:0]:0~7 对应页 0~7 
  * 00~0F 0 0 0 0 X3 X2 X1 X0 设置列地址(低四位) 设置8位起始列地址的低四位 
  * 10~1F 0 0 0 1 X3 X2 X1 X0 设置列地址(高四位) 设置8位起始列地址的高四位 
  */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{ 
	OLED_WR_Byte(0xb0 + y, OLED_CMD);            				//设置页地址
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);  	//设置列地址高4位
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);      		//设置列地址低4位
}   	  

/**
  * @brief  开启OLED显示
  * @param  无
  * @retval 无
  ******************************************************************************
  * @attention
  * SSD1306命令：
  *   8D   1 0 0 0 1  1 0 1
  * A[7:0] * * 0 1 0 A2 0 0  电荷泵设置   A2=0,关闭电荷泵;A2=1,开启电荷泵
  * AE/AF  1 0 1 0 1  1 1 X0 设置显示开关 X0=0,关闭显示;X0=1,开启显示
  */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON      //开启电荷泵
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON   //开启显示
}

/**
  * @brief  关闭OLED显示
  * @param  无
  * @retval 无
  */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF     //关闭电荷泵
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF  //关闭显示
}

/**
  * @brief  清屏函数
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{
	uint8_t i,n;		    
	for(i = 0; i < 8; i++)  
	{  
		OLED_WR_Byte(0xb0 + i, OLED_CMD);          //设置页地址（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);              //设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);              //设置显示位置—列高地址   
		for(n=0;n<128;n++) OLED_WR_Byte(0, OLED_DATA); 
	}                                            //更新显示
}

/**
  * @brief  在指定位置显示一个字符,包括部分字符
  * @param  uint8_t x, uint8_t y, uint8_t chr(x为列坐标，取值0~127；y为页坐标，取值0~7；chr为显示字符)
  * @retval 无
  */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{      	
	unsigned char c=0,i=0;	
	c = chr - ' ';                             //得到偏移后的值(对应ASCII码)			
	if(x > Max_Column - 1) {
	  x = 0;
	  if (SIZE == 16) y += 2;
	  else y++;
	}
	if (SIZE == 16)                            //一个字符占8列16行(2页) 一块128*64像素屏一行能显示16个字符 能显示4行
	{
		OLED_Set_Pos(x, y);	                     //开始写第一页
			                                       //写入字符前64列(第一页先列后行)
		for(i = 0; i < 8; i++)                   //每次写8位 写8次
			OLED_WR_Byte(F8X16[c*16 + i], OLED_DATA);   
		OLED_Set_Pos(x, y + 1);                  //开始写第二页
			                                       //写入字符后64列(第二页先列后行)
		for(i = 0; i < 8; i++)                   //每次写8位 写8次
			OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA); 
	}
	else
	{	
		OLED_Set_Pos(x, y);
		for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[c][i], OLED_DATA);
	}
}

/**
  * @brief  计算m^n
  * @param  uint8_t m, uint8_t n
  * @retval u32(返回m^n)
  */
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--) result*=m;    
	return result;
}				  

/**
  * @brief  显示1个数字
  * @param  uint8_t x, uint8_t y, uint32_t num, uint8_t len
  *         (x为列坐标，取值0~127；y为页坐标，取值0~7；num为所要显示的数字；len为数字长度；size为显示字体大小,默认填16)
  * @retval 无
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{         	
	uint8_t t, temp;
	uint8_t enshow = 0;
	for(t = 0; t < len; t++)
	{
		temp=(num / oled_pow(10, len-t-1)) % 10;	//从最高位依次将每一位(十进制)取出
		if(enshow == 0 && t < (len - 1))
		{
			if(temp == 0)
			{
			  if ( SIZE == 16 ) OLED_ShowChar(x + 8 * t, y, ' ');
			  else OLED_ShowChar(x + 6 * t, y, ' '); //将开头为0的位略成' '
				continue;
			}else enshow = 1; 
		}
		if ( SIZE == 16 ) OLED_ShowChar(x + 8 * t, y, temp + '0');	//打印数字
		else OLED_ShowChar(x + 6 * t, y, temp + '0'); //打印数字
	}
} 

/**
  * @brief  显示字符串
  * @param  uint8_t x, uint8_t y, uint8_t *chr
  *		    (x为列坐标，取值0~127；y为页坐标，取值0~3/7；*chr指针型存放chr[]
  * @retval 无
  */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr)
{
	uint8_t j=0;
	while(chr[j]!='\0')                         //该位不为空则打印
	{	
		OLED_ShowChar(x, y, chr[j]);        			//从第x列第y页打印字符
		if ( SIZE == 16 ) x += 8;                 //字宽8列 打印完跳8列
		else x += 6;                              //字宽6列 打印完跳6列
		if(x > 120){x = 0; y+=2;}                 //打印完2页则跳2页
			j++;
	}
}

/**
  * @brief  输出信息
  * @param  uint8_t *chr
  * @retval 无
  */
void OLED_TP78Info(uint8_t *chr)
{
  // 清空原有信息
  uint8_t i;
  OLED_Set_Pos(0, 3);
  for (i = 0; i < 64; i++) {
    OLED_WR_Byte(0x00, OLED_DATA);
  }
  // 定义左下角输出信息
  OLED_ShowString(OLED_Midx(strlen(chr), 0, 64), 3, chr);
}

/**
  * @brief  显示字符串
  * @param  uint8_t x, uint8_t y, uint8_t no(x为列坐标，取值0~127；y为页坐标，取值0~7；no为oledfont.h中存放汉字数组Hzk的序号)
  * @retval 无
  */
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)                           //打印中文第一页部分
    {
        OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
        adder+=1;
    }
	OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)                           //打印中文第二页部分
	{
		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
    }
}

/**
  * @brief  显示BMP图片
  * @param  uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]
  *         (x0为起始列坐标；y0为其实页坐标；x1为终止列坐标；y1位终止页坐标；BMP[]为存放图片的数组)
  * @retval 无
  */
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* BMP)
{ 	
 //BMP大小((x1-x0+1)*(y1-y0+1))
 unsigned int j=0;
 unsigned char x,y;

 for(y=y0;y<y1;y++)
 {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {
        OLED_WR_Byte(BMP[j++],OLED_DATA);	      //逐点显示，先行后列
    }
 }
} 

/**
  * @brief  填充区域
  * @param  uint8_t x0, uint8_t y0, uint8_tx1, uint8_t y1
  *         x0为起始列坐标；y0为其实页坐标；x1为终止列坐标；y1位终止页坐标
  * @retval 无
  */
void OLED_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
 //BMP大小((x1-x0+1)*(y1-y0+1))
 unsigned int j=0;
 unsigned char x,y;

 for(y=y0;y<y1;y++)
 {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {
        OLED_WR_Byte(0xFF, OLED_DATA);       //逐点显示，先行后列
    }
 }
}

/**
  * @brief  清空区域
  * @param  uint8_t x0, uint8_t y0, uint8_tx1, uint8_t y1
  *         x0为起始列坐标；y0为其实页坐标；x1为终止列坐标；y1位终止页坐标
  * @retval 无
  */
void OLED_Clr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
 //BMP大小((x1-x0+1)*(y1-y0+1))
 unsigned int j=0;
 unsigned char x,y;

 for(y=y0;y<y1;y++)
 {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {
        OLED_WR_Byte(0x00, OLED_DATA);       //逐点显示，先行后列
    }
 }
}

/**
  * @brief  OLED计算字符串/数字居中位置x坐标
  * @param  文字长度，显示区域起点/终点
  * @retval x坐标
  */
uint8_t OLED_Midx(uint8_t length, uint8_t xstart, uint8_t xend)
{
  uint8_t w = SIZE == 16 ? 8 : 6;
	if(length * w > xend - xstart) return 0;
	return xstart + (xend - xstart - length * w) / 2;
}

/**
  * @brief  OLED显示OK(打勾)
  * @param  x坐标, y坐标, 显示打勾或取消打勾
  * @retval 无
  */
void OLED_ShowOK(uint8_t x, uint8_t y, uint8_t s)
{
  uint8_t i;
  OLED_Set_Pos(x, y);
  if (SIZE == 8) {
    if (s != 0) for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[92][i], OLED_DATA);
    else for(i = 0; i < 6; i++) OLED_WR_Byte(0x00, OLED_DATA);
  }
}

/**
  * @brief  OLED显示Capslock(大小写)
  * @param  x坐标, y坐标, 显示大小写被按下或不显示
  * @retval 无
  */
void OLED_ShowCapslock(uint8_t x, uint8_t y, uint8_t s)
{
  uint8_t i;
  OLED_Set_Pos(x, y);
  if (SIZE == 8) {
    if (s != 0) for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[93][i], OLED_DATA);
    else for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[94][i], OLED_DATA);
  }
}

/**
  * @brief  OLED printf重定向函数
  * @param  x坐标, y坐标, 输出...
  * @retval 输出长度
  */
int OLED_printf(uint8_t x, uint8_t y, char *pFormat, ...)
{
  char pStr[16] = {'\0'};
  va_list ap;
  int res;

  va_start(ap, pFormat);
  res = vsprintf((char*)pStr, pFormat, ap);
  va_end(ap);

//  OLED_Clear();
//  OLED_ShowString(x, y, pStr);
  OLED_TP78Info(pStr);

  /* 记录至历史 */
  if (strlen(pStr) > OLED_HIS_DLEN) { // 截取长度
    pStr[OLED_HIS_DLEN-1] = '\0';
  }
  if (OLED_printf_history_idx >= OLED_HIS_LEN) {  // 缓存满 - 队列结构
    memcpy((uint8_t*)OLED_printf_history[0], (uint8_t*)OLED_printf_history[1], (OLED_HIS_LEN-1)*(OLED_HIS_DLEN));
    strcpy(OLED_printf_history[OLED_HIS_LEN-1], pStr);
  } else {
    OLED_printf_history_idx++;
  }

  return res;
}

/**
  * @brief  初始化SSD1306
  * @param  无
  * @retval 无
  */
void HAL_OLED_Init(void)
{ 	
  //IO
  OLED_CLK_GPIO_(SetBits)( OLED_CLK_Pin );
  OLED_SDA_GPIO_(SetBits)( OLED_SDA_Pin );
  OLED_CLK_GPIO_(ModeCfg)( OLED_CLK_Pin, GPIO_ModeOut_PP_5mA );
  OLED_SDA_GPIO_(ModeCfg)( OLED_SDA_Pin, GPIO_ModeOut_PP_5mA );

  DelayMs(50);  //200

  OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel

  OLED_WR_Byte(0x40,OLED_CMD);//---set low column address
  OLED_WR_Byte(0xB0,OLED_CMD);//---set high column address

  OLED_WR_Byte(0xC8,OLED_CMD);//-not offset

  OLED_WR_Byte(0x81,OLED_CMD);//设置对比度
  OLED_WR_Byte(0xff,OLED_CMD);

  OLED_WR_Byte(0xa1,OLED_CMD);//段重定向设置

  OLED_WR_Byte(0xa6,OLED_CMD);//

  OLED_WR_Byte(0xa8,OLED_CMD);//设置驱动路数
  OLED_WR_Byte(0x1f,OLED_CMD);

  OLED_WR_Byte(0xd3,OLED_CMD);
  OLED_WR_Byte(0x00,OLED_CMD);

  OLED_WR_Byte(0xd5,OLED_CMD);
  OLED_WR_Byte(0xf0,OLED_CMD);

  OLED_WR_Byte(0xd9,OLED_CMD);
  OLED_WR_Byte(0x22,OLED_CMD);

  OLED_WR_Byte(0xda,OLED_CMD);
  OLED_WR_Byte(0x02,OLED_CMD);

  OLED_WR_Byte(0xdb,OLED_CMD);
  OLED_WR_Byte(0x49,OLED_CMD);

  OLED_WR_Byte(0x8d,OLED_CMD);
  OLED_WR_Byte(0x14,OLED_CMD);
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}
/*********************************************END OF FILE**********************/
