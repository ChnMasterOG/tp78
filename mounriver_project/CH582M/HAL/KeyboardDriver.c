/********************************** (C) COPYRIGHT *******************************
 * File Name          : KeyboardDriver.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/19
 * Description        : 机械键盘驱动源文件
 *******************************************************************************/

#include "KeyboardDriver.h"
#include "USBDriver.h"

const uint32_t Row_GPIO_Pin[] = {GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13};
const uint32_t Colum_GPIO_Pin[] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6,
                                   GPIO_Pin_7, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
const uint16_t KeyArrary[sizeof(Colum_GPIO_Pin)/sizeof(uint32_t)][sizeof(Row_GPIO_Pin)/sizeof(uint32_t)];

Keyboardstate* const Keyboarddat = (Keyboardstate*)HIDKey;



