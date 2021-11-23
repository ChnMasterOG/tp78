/********************************** (C) COPYRIGHT *******************************
 * File Name          : KeyboardDriver.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/19
 * Description        : 机械键盘驱动源文件
 *******************************************************************************/

#include "KeyboardDriver.h"
#include "USBDriver.h"

const uint32_t Row_Pin[] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_16, GPIO_Pin_17, GPIO_Pin_18, GPIO_Pin_19};   //6
const uint32_t Colum_Pin[] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6,
                              GPIO_Pin_7, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};   //14
//row*colum = 6*14 = 84
const uint16_t KeyArrary[sizeof(Colum_Pin)/sizeof(uint32_t)][sizeof(Row_Pin)/sizeof(uint32_t)] = {
        { KEY_ESCAPE,   KEY_GraveAccent,    KEY_TAB,        KEY_CapsLock,   KEY_LeftShift,  KEY_LeftCTRL }, //1
        { KEY_F1,       KEY_1,              KEY_Q,          KEY_A,          KEY_Z,          KEY_LeftGUI  }, //2
        { KEY_F2,       KEY_2,              KEY_W,          KEY_S,          KEY_X,          KEY_LeftAlt  }, //3
        { KEY_F3,       KEY_3,              KEY_E,          KEY_D,          KEY_C,          KEY_None     }, //4
        { KEY_F4,       KEY_4,              KEY_R,          KEY_F,          KEY_V,          KEY_SPACEBAR }, //5
        { KEY_F5,       KEY_5,              KEY_T,          KEY_G,          KEY_B,          KEY_None     }, //6
        { KEY_F6,       KEY_6,              KEY_Y,          KEY_None,       KEY_None,       KEY_None     }, //7
        { KEY_F7,       KEY_7,              KEY_U,          KEY_H,          KEY_None,       KEY_None     }, //8
        { KEY_F8,       KEY_8,              KEY_I,          KEY_J,          KEY_N,          KEY_None     }, //9
        { KEY_F9,       KEY_9,              KEY_O,          KEY_K,          KEY_M,          KEY_None     }, //10
        { KEY_F10,      KEY_0,              KEY_P,          KEY_L,          KEY_Comma,      KEY_RightAlt }, //11
        { KEY_F11,      KEY_Subtraction,    KEY_LSbrackets, KEY_Semicolon,  KEY_FullStop,   KEY_RightGUI }, //12
        { KEY_F12,      KEY_Equal,          KEY_RSbrackets, KEY_Quotation,  KEY_Slash,      KEY_Fn       }, //13
        { KEY_Delete,   KEY_BACKSPACE,      KEY_Backslash,  KEY_ENTER,      KEY_RightShift, KEY_RightCTRL}, //14
};
uint16_t CustomKey[sizeof(Colum_Pin)/sizeof(uint32_t)][sizeof(Row_Pin)/sizeof(uint32_t)];
uint32_t Row_Pin_ALL = 0, Colum_Pin_ALL = 0;
/* 记录按下键的信息 */
uint8_t press_row[MAX_PRESS_COUNT] = { 0xFF }, press_colum[MAX_PRESS_COUNT] = { 0xFF };
uint8_t dataIndex_to_commonIndex[MAX_PRESS_COUNT] = { 0xFF };

uint8_t is_press = 0,
        KEY_data_index = 0,
        KEY_common_index = 2,
        KEY_data_ready = 0,
        Fn_state = 0;
Keyboardstate* const Keyboarddat = (Keyboardstate*)HIDKey;

void KEY_Init(void)
{
    uint8_t i;
    memcpy(CustomKey, KeyArrary, sizeof(KeyArrary));    // KeyArray是默认按键，初始为默认按键
    for (i = 0; i < sizeof(Row_Pin)/sizeof(uint32_t); i++) {
        Row_GPIO_(ModeCfg)( Row_Pin[i], GPIO_ModeOut_PP_5mA );
        Row_Pin_ALL |= Row_Pin[i];
    }
    Row_GPIO_(SetBits)( Row_Pin_ALL );
    for (i = 0; i < sizeof(Colum_Pin)/sizeof(uint32_t); i++) {
        Colum_GPIO_(ModeCfg)( Colum_Pin[i], GPIO_ModeIN_PD );
        Colum_Pin_ALL |= Colum_Pin[i];
    }
    R16_PA_INT_MODE |= Colum_Pin_ALL;
    Colum_GPIO_(ITModeCfg)( Colum_Pin_ALL, GPIO_ITMode_RiseEdge );   // 只检测按下
    PFIC_EnableIRQ( GPIO_A_IRQn );  // Colum_GPIO
}

void KEY_FallEdge_detection(void)
{
    uint8_t press_count = KEY_data_index;
    // 检查是否有键松开
    for (int i = 0; i < press_count; i++) {
        if (press_colum[i] != 0xFF && Colum_GPIO_(ReadPortPin)( Colum_Pin[press_colum[i]] ) == 0) {
            KEY_data_ready = 1;
            if (CustomKey[press_colum[i]][press_row[i]] == KEY_Fn) {  // 功能键
                Fn_state = 0;
            } else if (CustomKey[press_colum[i]][press_row[i]] >= KEY_LeftCTRL) {    // 特殊键
                Keyboarddat->data[0] &= ~(1 << (CustomKey[press_colum[i]][press_row[i]] - KEY_LeftCTRL));
            } else {
                memcpy(&Keyboarddat->data[dataIndex_to_commonIndex[i]], &Keyboarddat->data[dataIndex_to_commonIndex[i]] + 1, 7 - dataIndex_to_commonIndex[i]);
                Keyboarddat->Key6 = 0;
                dataIndex_to_commonIndex[i] = 0;
                KEY_common_index--;
            }
            press_colum[i] = press_row[i] = 0xFF;
            KEY_data_index--;
        }
    }
}

void KEY_RisingEdge_detection(void)
{
    uint8_t row_index, colum_index;
    uint8_t state, mask;
    if (is_press == 0) return;
    is_press = 0;
    R16_PA_INT_EN &= ~Colum_Pin_ALL; // 关中断
    Row_GPIO_(ResetBits)( Row_Pin_ALL );
    GPIOB_InverseBits( GPIO_Pin_0 );
    for (row_index = 0; row_index < sizeof(Row_Pin)/sizeof(uint32_t); row_index++) {  // 扫描所有行
        Row_GPIO_(SetBits)( Row_Pin[row_index] );
        for (colum_index = 0; colum_index < sizeof(Colum_Pin)/sizeof(uint32_t); colum_index++) {    // 查询哪一列发生中断
            if (Colum_GPIO_(ReadPortPin)( Colum_Pin[colum_index] )) {  // 上升沿
                if (KEY_common_index >= 8 && CustomKey[colum_index][row_index] < KEY_LeftCTRL) continue;    // 超过6个按键上限
                press_row[KEY_data_index] = row_index;
                press_colum[KEY_data_index] = colum_index;
                if (CustomKey[colum_index][row_index] == KEY_Fn) {  // 功能键
                    Fn_state = 1;
                } else if (CustomKey[colum_index][row_index] >= KEY_LeftCTRL) {    // 特殊键
                    Keyboarddat->data[0] |= 1 << (CustomKey[colum_index][row_index] - KEY_LeftCTRL);
                } else {
                    dataIndex_to_commonIndex[KEY_data_index] = KEY_common_index;
                    Keyboarddat->data[KEY_common_index++] = CustomKey[colum_index][row_index];
                }
                KEY_data_index++;
            }
        }
        Row_GPIO_(ResetBits)( Row_Pin[row_index] );
    }
    Row_GPIO_(SetBits)( Row_Pin_ALL );
    R16_PA_INT_IF = Colum_Pin_ALL;
    R16_PA_INT_EN |= Colum_Pin_ALL;  // 开中断
    KEY_data_ready = 1;
}

void KEY_EX_IT_handler(void)
{
    if ( Colum_GPIO_(ReadITFlagBit)( Colum_Pin_ALL ) != 0 ) {  // 一行任意按键按下
        Colum_GPIO_(ClearITFlagBit)( Colum_Pin_ALL );
        is_press = 1;
    }
}

