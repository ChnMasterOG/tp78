/********************************** (C) COPYRIGHT *******************************
 * File Name          : KeyboardDriver.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/19
 * Description        : 机械键盘驱动源文件
 *******************************************************************************/

#include "KeyboardDriver.h"
#include "USBDriver.h"

const uint32_t Row_Pin[] = {GPIO_Pin_7, GPIO_Pin_6, GPIO_Pin_5, GPIO_Pin_4, GPIO_Pin_3, GPIO_Pin_2};   //row 6
const uint32_t Colum_Pin[] = {GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3,
                              GPIO_Pin_15, GPIO_Pin_14, GPIO_Pin_13, GPIO_Pin_12, GPIO_Pin_11, GPIO_Pin_10, GPIO_Pin_7};   //colum 14
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
uint8_t KeyMatrix[sizeof(Colum_Pin)/sizeof(uint32_t)][sizeof(Row_Pin)/sizeof(uint32_t)] = { 0 };
uint32_t Row_Pin_ALL = 0, Colum_Pin_ALL = 0;

uint8_t KEY_data_index = 2,
        KEY_data_ready = 0,
        Fn_state = 0;
Keyboardstate* const Keyboarddat = (Keyboardstate*)HIDKey;

void KEY_Init(void)
{
    uint8_t i;
    memcpy(CustomKey, KeyArrary, sizeof(KeyArrary));    // KeyArray是默认按键，初始为默认按键
    for (i = 0; i < sizeof(Row_Pin)/sizeof(uint32_t); i++) {
        Row_Pin_ALL |= Row_Pin[i];
    }
    Row_GPIO_(SetBits)( Row_Pin_ALL );
    Row_GPIO_(ModeCfg)( Row_Pin_ALL, GPIO_ModeOut_PP_20mA );
    Row_GPIO_(SetBits)( Row_Pin_ALL & (~Row_Pin[0]) );

    for (i = 0; i < sizeof(Colum_Pin)/sizeof(uint32_t); i++) {
        Colum_Pin_ALL |= Colum_Pin[i];
    }
    Row_GPIO_(SetBits)( Colum_Pin_ALL );
    Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeIN_PU );
}

void KEY_detection(void)
{
    static uint8_t current_row = 0;
    uint8_t current_colum, key_idx;
    for (current_colum = 0; current_colum < sizeof(Colum_Pin)/sizeof(uint32_t); current_colum++) {    // 查询哪一列改变
        if (KeyMatrix[current_colum][current_row] == 0 && Colum_GPIO_(ReadPortPin)( Colum_Pin[current_colum] ) == 0) {  // 按下
            if (KEY_data_index >= 8 && CustomKey[current_colum][current_row] < KEY_LeftCTRL) continue;    // 超过6个普通按键上限
            KeyMatrix[current_colum][current_row] = 1;  // 矩阵状态变成按下
            KEY_data_ready = 1; // 产生事件
            if (CustomKey[current_colum][current_row] == KEY_Fn) {  // 功能键
                Fn_state = 1;
            } else if (CustomKey[current_colum][current_row] >= KEY_LeftCTRL) {    // 特殊键
                Keyboarddat->data[0] |= 1 << (CustomKey[current_colum][current_row] - KEY_LeftCTRL);
            } else {
                Keyboarddat->data[KEY_data_index++] = CustomKey[current_colum][current_row];
            }
        } else if (KeyMatrix[current_colum][current_row] != 0 && Colum_GPIO_(ReadPortPin)( Colum_Pin[current_colum] ) != 0) {   // 弹起
            KeyMatrix[current_colum][current_row] = 0;
            KEY_data_ready = 1; // 产生事件
            if (CustomKey[current_colum][current_row] == KEY_Fn) {  // 功能键
                Fn_state = 0;
            } else if (CustomKey[current_colum][current_row] >= KEY_LeftCTRL) {    // 特殊键
                Keyboarddat->data[0] &= ~(1 << (CustomKey[current_colum][current_row] - KEY_LeftCTRL));
            } else {
                for (key_idx = 2; key_idx < 8; key_idx++) {
                    if (Keyboarddat->data[key_idx] == CustomKey[current_colum][current_row]) {
                        memcpy(&Keyboarddat->data[key_idx], &Keyboarddat->data[key_idx] + 1, 7 - key_idx);
                        Keyboarddat->Key6 = 0;
                    }
                }
                KEY_data_index--;
            }
        }
    }
    Row_GPIO_(SetBits)( Row_Pin[current_row++] );
    if (current_row >= sizeof(Row_Pin)/sizeof(uint32_t)) {
        current_row = 0;
    }
    Row_GPIO_(ResetBits)( Row_Pin[current_row] );
}


