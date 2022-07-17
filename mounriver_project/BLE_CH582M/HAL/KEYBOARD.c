/********************************** (C) COPYRIGHT *******************************
 * File Name          : KEYBOARD.c
 * Author             : ChnMasterOG
 * Version            : V1.5
 * Date               : 2022/3/19
 * Description        : 机械键盘驱动源文件
 *******************************************************************************/

#include "HAL.h"

/* 彩蛋 */
#include "snake.h"

const uint32_t Row_Pin[ROW_SIZE] = {GPIO_Pin_7, GPIO_Pin_6, GPIO_Pin_5, GPIO_Pin_4, GPIO_Pin_3, GPIO_Pin_2};   //row 6 - 其它键盘布局需修改此处
const uint32_t Colum_Pin[COL_SIZE] = {GPIO_Pin_9, GPIO_Pin_7, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_4, GPIO_Pin_5,
                                      GPIO_Pin_6, GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_15, GPIO_Pin_14};   //colum 14 - 其它键盘布局需修改此处
//row*colum = 6*14 = 84
const uint8_t KeyArrary[COL_SIZE][ROW_SIZE] = {
        { KEY_ESCAPE,   KEY_GraveAccent,    KEY_TAB,        KEY_CapsLock,   KEY_LeftShift,  KEY_LeftCTRL }, //1
        { KEY_F1,       KEY_1,              KEY_Q,          KEY_A,          KEY_None,       KEY_LeftGUI  }, //2
        { KEY_F2,       KEY_2,              KEY_W,          KEY_S,          KEY_Z,          KEY_LeftAlt  }, //3
        { KEY_F3,       KEY_3,              KEY_E,          KEY_D,          KEY_X,          KEY_MouseL   }, //4
        { KEY_F4,       KEY_4,              KEY_R,          KEY_F,          KEY_C,          KEY_MouseM   }, //5
        { KEY_F5,       KEY_5,              KEY_T,          KEY_G,          KEY_V,          KEY_MouseR   }, //6
        { KEY_F6,       KEY_6,              KEY_Y,          KEY_H,          KEY_B,          KEY_SPACEBAR }, //7
        { KEY_F7,       KEY_7,              KEY_U,          KEY_J,          KEY_N,          KEY_None     }, //8
        { KEY_F8,       KEY_8,              KEY_I,          KEY_K,          KEY_M,          KEY_None     }, //9
        { KEY_F9,       KEY_9,              KEY_O,          KEY_L,          KEY_Comma,      KEY_None     }, //10
        { KEY_F10,      KEY_0,              KEY_P,          KEY_Semicolon,  KEY_FullStop,   KEY_RightAlt }, //11
        { KEY_F11,      KEY_Subtraction,    KEY_LSbrackets, KEY_Quotation,  KEY_Slash,      KEY_RightGUI }, //12
        { KEY_F12,      KEY_Equal,          KEY_RSbrackets, KEY_None,       KEY_None,       KEY_Fn       }, //13
        { KEY_Delete,   KEY_BACKSPACE,      KEY_NonUS_WS,   KEY_ENTER,      KEY_RightShift, KEY_RightCTRL}, //14
};  // 默认键盘布局 - 其它键盘布局需修改此处
const uint8_t Extra_KeyArrary[COL_SIZE][ROW_SIZE] = {
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_None,       KEY_None }, //1
        { KEY_None,     KEY_None,           KEY_BACKSPACE,  KEY_LeftArrow,  KEY_None,       KEY_None }, //2
        { KEY_None,     KEY_None,           KEY_UpArrow,    KEY_DownArrow,  KEY_None,       KEY_None }, //3
        { KEY_None,     KEY_None,           KEY_ENTER,      KEY_RightArrow, KEY_None,       KEY_None }, //4
        { KEY_None,     KEY_None,           KEY_PageUp,     KEY_PageDown,   KEY_Home,       KEY_None }, //5
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_End,        KEY_None }, //6
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_None,       KEY_None }, //7
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_None,       KEY_None }, //8
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_None,       KEY_None }, //9
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_None,       KEY_None }, //10
        { KEY_None,     KEY_None,           KEY_PrintScreen,KEY_None,       KEY_None,       KEY_None }, //11
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_None,       KEY_None }, //12
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_None,       KEY_None }, //13
        { KEY_None,     KEY_None,           KEY_None,       KEY_None,       KEY_None,       KEY_None }, //14
};  // 额外默认键盘布局 - 其它键盘布局需修改此处
const uint8_t Key_To_LEDNumber[COL_SIZE][ROW_SIZE] = {
        { 61,   47,   33,   20,   8,    0   }, //1
        { 62,   48,   34,   21,   0xFF, 1   }, //2
        { 63,   49,   35,   22,   9,    2   }, //3
        { 64,   50,   36,   23,   10,   0xFF}, //4
        { 65,   51,   37,   24,   11,   0xFF}, //5
        { 66,   52,   38,   25,   12,   0xFF}, //6
        { 67,   53,   39,   26,   13,   3   }, //7
        { 68,   54,   40,   27,   14,   0xFF}, //8
        { 69,   55,   41,   28,   15,   0xFF}, //9
        { 70,   56,   42,   29,   16,   0xFF}, //10
        { 71,   57,   43,   30,   17,   4   }, //11
        { 72,   58,   44,   31,   18,   5   }, //12
        { 73,   59,   45,   0xFF, 0xFF, 6   }, //13
        { 74,   60,   46,   32,   19,   7   }, //14
};  // 矩阵键盘位置转LED编号
uint8_t CustomKey[COL_SIZE][ROW_SIZE];  //自定义按键层
uint8_t Extra_CustomKey[COL_SIZE][ROW_SIZE];   //自定义额外按键层
uint8_t KeyMatrix[COL_SIZE][ROW_SIZE] = { 0 };  //按键矩阵-标记按下和未按下
uint8_t Extra_KeyMatrix[COL_SIZE][ROW_SIZE] = { 0 };  //额外层按键矩阵-标记按下和未按下
uint32_t Row_Pin_ALL = 0, Colum_Pin_ALL = 0;

uint8_t KEYBOARD_data_index = 2,
        KEYBOARD_data_ready = 0,
        KEYBOARD_mouse_ready = 0,
        LED_Change_flag = 0,
        Fn_state = 0;
Keyboardstate* const Keyboarddat = (Keyboardstate*)HIDKey;
BOOL PaintedEggMode = FALSE;
static uint8_t (*KeyArr_Ptr)[ROW_SIZE] = CustomKey;
static uint16_t KeyArr_ChangeTimes = 0;

/*******************************************************************************
* Function Name  : FLASH_Read_KeyArray
* Description    : 从Flash读取按键矩阵
* Input          : None
* Return         : None
*******************************************************************************/
void FLASH_Read_KeyArray( void )
{
  EEPROM_READ( FLASH_ADDR_CustomKey, CustomKey, COL_SIZE*ROW_SIZE );
  EEPROM_READ( FLASH_ADDR_Extra_CustomKey, Extra_CustomKey, COL_SIZE*ROW_SIZE );
}

/*******************************************************************************
* Function Name  : FLASH_Write_KeyArray
* Description    : 将按键矩阵写入Flash
* Input          : None
* Return         : 如果成功返回0
*******************************************************************************/
UINT8 FLASH_Write_KeyArray( void )
{
  UINT8 s;
  s = EEPROM_WRITE( FLASH_ADDR_CustomKey, CustomKey, COL_SIZE*ROW_SIZE );
  s |= EEPROM_WRITE( FLASH_ADDR_Extra_CustomKey, Extra_CustomKey, COL_SIZE*ROW_SIZE );
  return s;
}

/*******************************************************************************
* Function Name  : KEYBOARD_Reset
* Description    : 重置FLASH
* Input          : None
* Return         : None
*******************************************************************************/
void KEYBOARD_Reset( void )
{
  uint8_t temp;
  memcpy(CustomKey, KeyArrary, COL_SIZE*ROW_SIZE);
  memcpy(Extra_CustomKey, Extra_KeyArrary, COL_SIZE*ROW_SIZE);
  FLASH_Write_KeyArray( );
  temp = 0;
  EEPROM_WRITE( FLASH_ADDR_LEDStyle, &temp, 1 );   // default LED Style
  temp = 1;
  EEPROM_WRITE( FLASH_ADDR_BLEDevice, &temp, 1 );   // default BLE Device
}

/*******************************************************************************
* Function Name  : KEYBOARD_ChangeKey
* Description    : 键盘交换2个按键
* Input          : dst_key - 目标键, src_key - 原始键
* Return         : None
*******************************************************************************/
void KEYBOARD_ChangeKey( uint8_t dst_key, uint8_t src_key )
{
  uint8_t i = COL_SIZE*ROW_SIZE;
  uint8_t* memaddr = &CustomKey[0][0];
  while (i) {
    --i;
    /* 交换src键和dst键 */
    if ( *(memaddr + i) == src_key ) *(memaddr + i) = dst_key;
    else if ( *(memaddr + i) == dst_key ) *(memaddr + i) = src_key;
  }
  FLASH_Write_KeyArray( );
}

/*******************************************************************************
* Function Name  : KEYBOARD_Custom_Function
* Description    : 键盘定制化功能(检测Fn键触发的相关功能)
* Input          : None
* Return         : 如果不需要发送按键信息则返回0
*******************************************************************************/
UINT8 KEYBOARD_Custom_Function( void )
{
  static uint8_t Fn_Mode = 0;
  static uint8_t last_state = 0;
  static uint8_t Fn_cnt = 0;  // 4/2/1/1 bit for Reset/ChangeKey/SaveDevices/Reserve
  static uint8_t src_key = 0, dst_key = 0;
  if ( Fn_state != 0 && Keyboarddat->Key2 == KEY_None ) { // 按下Fn+1个键或者只有Fn键
    last_state = 1;
    if ( Keyboarddat->Key1 == KEY_None ) { // 键盘布局复位
      if ( Fn_Mode == Fn_Mode_None ) {
        Fn_Mode = Fn_Mode_Reset;
        Fn_cnt &= 0xF0;   // 清除其余计数器
      }
    } else if ( Fn_Mode == Fn_Mode_ChangeKey ) {  // 设置改键 - 按Fn+src和Fn+dst
      if ( Fn_cnt == 0x04 ) src_key = Keyboarddat->Key1;
      else if ( Fn_cnt == 0x08 ) dst_key = Keyboarddat->Key1;
    } else if ( Keyboarddat->Key1 == KEY_C && Fn_Mode != Fn_Mode_ChangeKey ) { // 设置改键 - 先按Fn+C
      Fn_Mode = Fn_Mode_ChangeKey;
      Fn_cnt &= 0x0C;
    } else if ( Keyboarddat->Key1 == KEY_B && Fn_Mode != Fn_Mode_JumpBoot ) { // 跳转BOOT模式
      Fn_Mode = Fn_Mode_JumpBoot;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_Delete && Fn_Mode != Fn_Mode_PaintedEgg ) { // 彩蛋模式
      Fn_Mode = Fn_Mode_PaintedEgg;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_Equal && Fn_Mode != Fn_Mode_DisEnableBLE ) { // 使能/失能蓝牙
      Fn_Mode = Fn_Mode_DisEnableBLE;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_0 && Fn_Mode != Fn_Mode_PriorityUSBorBLE ) { // 设置优先蓝牙或USB
      Fn_Mode = Fn_Mode_PriorityUSBorBLE;
      Fn_cnt = 0;
    }  else if ( Keyboarddat->Key1 == KEY_1 && Fn_Mode != Fn_Mode_SelectDevice1 ) { // 切换至设备1
      Fn_Mode = Fn_Mode_SelectDevice1;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_2 && Fn_Mode != Fn_Mode_SelectDevice2 ) { // 切换至设备2
      Fn_Mode = Fn_Mode_SelectDevice2;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_3 && Fn_Mode != Fn_Mode_SelectDevice3 ) { // 切换至设备3
      Fn_Mode = Fn_Mode_SelectDevice3;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_4 && Fn_Mode != Fn_Mode_SelectDevice4 ) { // 切换至设备4
      Fn_Mode = Fn_Mode_SelectDevice4;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_5 && Fn_Mode != Fn_Mode_SelectDevice5 ) { // 切换至设备5
      Fn_Mode = Fn_Mode_SelectDevice5;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_6 && Fn_Mode != Fn_Mode_SelectDevice6 ) { // 切换至设备6
      Fn_Mode = Fn_Mode_SelectDevice6;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_F1 && Fn_Mode != Fn_Mode_LED_Style1 ) { // 呼吸灯模式1
      Fn_Mode = Fn_Mode_LED_Style1;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_F2 && Fn_Mode != Fn_Mode_LED_Style2 ) { // 呼吸灯模式2
      Fn_Mode = Fn_Mode_LED_Style2;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_F3 && Fn_Mode != Fn_Mode_LED_Style3 ) { // 呼吸灯模式3
      Fn_Mode = Fn_Mode_LED_Style3;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_F4 && Fn_Mode != Fn_Mode_LED_Style4 ) { // 呼吸灯模式4
      Fn_Mode = Fn_Mode_LED_Style4;
      Fn_cnt = 0;
    } else if ( Keyboarddat->Key1 == KEY_F5 && Fn_Mode != Fn_Mode_LED_Style5 ) { // 呼吸灯模式5
      Fn_Mode = Fn_Mode_LED_Style5;
      Fn_cnt = 0;
    } else {  // 不识别的指令
      Fn_Mode = Fn_Mode_None;
      Fn_cnt = 0;
    }
    return 0;
  } else if ( last_state == 1 && Fn_state == 0 ) {  // 松开Fn执行指令
    last_state = 0;
    switch ( Fn_Mode ) {
      case Fn_Mode_Reset: // 按下5次Fn重置按键
        Fn_cnt += 0x10;
        if ( Fn_cnt == 0x50 ) {
          Fn_cnt = 0;
          Fn_Mode = Fn_Mode_None;
          KEYBOARD_Reset( );
          OLED_PRINT("Reset OK!");
          tmos_start_task( halTaskID, OLED_EVENT, MS1_TO_SYSTEM_TIME(3000) );
        }
        break;
      case Fn_Mode_ChangeKey:
        Fn_cnt += 0x04;
        if ( Fn_cnt == 0x0C ) {
          Fn_cnt = 0;
          Fn_Mode = Fn_Mode_None;
          OLED_PRINT("Change OK!");
          tmos_start_task( halTaskID, OLED_EVENT, MS1_TO_SYSTEM_TIME(3000) );
          KEYBOARD_ChangeKey( dst_key, src_key );
        }
        break;
      case Fn_Mode_JumpBoot:  // Fn+B跳转BOOT
        Fn_Mode = Fn_Mode_None;
        APPJumpBoot();
        break;
      case Fn_Mode_PaintedEgg:  // Fn+Delete彩蛋
        Fn_Mode = Fn_Mode_None;
        if (PaintedEggMode == FALSE) {
          LED_Change_flag = 1;
          led_style_func = WS2812_Style_Custom; // 彩蛋背光
          Snake_Init();
        }
        PaintedEggMode = !PaintedEggMode;
        break;
      case Fn_Mode_DisEnableBLE:
        Fn_Mode = Fn_Mode_None; // Fn+等号关闭/开启蓝牙
        extern BOOL enable_BLE;
        if ( !BLE_Ready ) {
          enable_BLE = !enable_BLE;
          bStatus_t status = GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &enable_BLE );
          if ( status != SUCCESS ) OLED_PRINT("ERR %d", status);
          else if ( enable_BLE ) OLED_PRINT("BLE ENA");
          else OLED_PRINT("BLE DIS");
          tmos_start_task( halTaskID, OLED_EVENT, MS1_TO_SYSTEM_TIME(3000) );
        }
        break;
      case Fn_Mode_PriorityUSBorBLE:  // Fn+0优先蓝牙或USB切换
        Fn_Mode = Fn_Mode_None;
        extern BOOL priority_USB;
        priority_USB = !priority_USB;
        if ( USB_Ready && BLE_Ready ) {
          OLED_ShowOK(26 + priority_USB * 30, 0, FALSE);
          OLED_ShowOK(26 + !priority_USB * 30, 0, TRUE);
        }
        if ( priority_USB ) OLED_PRINT("PRI USB");
        else OLED_PRINT("PRI BLE");
        tmos_start_task( halTaskID, OLED_EVENT, MS1_TO_SYSTEM_TIME(3000) );
        break;
      case Fn_Mode_SelectDevice1 ... Fn_Mode_SelectDevice6: // 按Fn+1~6切换设备
        DeviceAddress[5] = Fn_Mode - Fn_Mode_SelectDevice1 + 1;
        Fn_Mode = Fn_Mode_None;
        tmos_start_task( hidEmuTaskId, CHANGE_ADDR_EVT, 500 );
        break;
      case Fn_Mode_LED_Style1:
        Fn_Mode = Fn_Mode_None;
        LED_Change_flag = 1;
        OLED_ShowString(20, 1, "S0");
        led_style_func = WS2812_Style_Off;  // Fn+1 - 关闭背光
        break;
      case Fn_Mode_LED_Style2:
        Fn_Mode = Fn_Mode_None;
        LED_Change_flag = 1;
        OLED_ShowString(20, 1, "S1");
        led_style_func = WS2812_Style_Breath;  // Fn+2 - 背光使用呼吸灯模式
        break;
      case Fn_Mode_LED_Style3:
        Fn_Mode = Fn_Mode_None;
        LED_Change_flag = 1;
        OLED_ShowString(20, 1, "S2");
        led_style_func = WS2812_Style_Waterful;  // Fn+3 - 背光使用流水灯模式
        break;
      case Fn_Mode_LED_Style4:
        Fn_Mode = Fn_Mode_None;
        LED_Change_flag = 1;
        OLED_ShowString(20, 1, "S3");
        led_style_func = WS2812_Style_Touch;  // Fn+4 - 背光使用触控呼吸灯模式
        break;
      case Fn_Mode_LED_Style5:
        Fn_Mode = Fn_Mode_None;
        LED_Change_flag = 1;
        OLED_ShowString(20, 1, "S4");
        led_style_func = WS2812_Style_Rainbow;  // Fn+5 - 背光使用彩虹灯模式
        break;
      case Fn_Mode_GiveUp:
        Fn_cnt = 0;
        Fn_Mode = Fn_Mode_None;
        break;
      default:  // 未识别的指令
        ;
    }
    return 0;
  }
  return 1;
}

/*******************************************************************************
* Function Name  : KEYBOARD_Init
* Description    : 键盘初始化函数
* Input          : None
* Return         : None
*******************************************************************************/
void KEYBOARD_Init( void )
{
    uint8_t i;
    FLASH_Read_KeyArray( );   // Flash载入按键
    CustomKey[12][5] = KEY_Fn;   // 保证上电Fn键在对应位置 - 其它键盘布局需修改此处
    for (i = 0; i < ROW_SIZE; i++) {
        Row_Pin_ALL |= Row_Pin[i];
    }
    Row_GPIO_(SetBits)( Row_Pin_ALL );
    Row_GPIO_(ModeCfg)( Row_Pin_ALL, GPIO_ModeOut_PP_20mA );
    Row_GPIO_(SetBits)( Row_Pin_ALL & (~Row_Pin[0]) );

    for (i = 0; i < COL_SIZE; i++) {
        Colum_Pin_ALL |= Colum_Pin[i];
    }
    Colum_GPIO_(SetBits)( Colum_Pin_ALL );
    Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeIN_PU );
}

/*******************************************************************************
* Function Name  : KEYBOARD_Detection
* Description    : 键盘检测按键信息函数
* Input          : None
* Return         : None
*******************************************************************************/
void KEYBOARD_Detection( void )
{
    static uint8_t current_row = 0;
    static BOOL press_Capslock = FALSE;
    static BOOL press_NormalKey = FALSE;
    uint8_t current_colum, key_idx;
    if (KeyArr_ChangeTimes > 0 && KeyArr_ChangeTimes <= MAX_CHANGETIMES) {  // 进入CapsLock键盘布局改变计数等待
        if (KeyArr_ChangeTimes == MAX_CHANGETIMES) { // 计数值到达MAX_CHANGETIMES改变键盘布局
            KeyArr_Ptr = Extra_CustomKey;
            OLED_ShowString(2, 1, "L2");
        }
        ++KeyArr_ChangeTimes; // 键盘计数值递增
    } else if (press_Capslock) {  // CapsLock被按下
        press_Capslock = FALSE;
        for (key_idx = 2; key_idx < 8; key_idx++) {
            if (Keyboarddat->data[key_idx] == KEY_CapsLock) {
                memcpy(&Keyboarddat->data[key_idx], &Keyboarddat->data[key_idx] + 1, 7 - key_idx);
                Keyboarddat->Key6 = 0;
            }
        }
        KEYBOARD_data_index--;
        KEYBOARD_data_ready = 1;  // 产生事件
        return;
    }
    for (current_colum = 0; current_colum < COL_SIZE; current_colum++) {    // 查询哪一列改变
        if (KeyMatrix[current_colum][current_row] == 0 && Colum_GPIO_(ReadPortPin)( Colum_Pin[current_colum] ) == 0) {  // 按下
            if (KEYBOARD_data_index >= 8 && KeyArr_Ptr[current_colum][current_row] < KEY_LeftCTRL) continue;    // 超过6个普通按键上限
            KeyMatrix[current_colum][current_row] = 1;  // 矩阵状态变成按下
            if (led_style_func == WS2812_Style_Touch && Key_To_LEDNumber[current_colum][current_row] != 0xFF) { // 触控呼吸灯模式
                LED_BYTE_Buffer[Key_To_LEDNumber[current_colum][current_row]][0] =
                LED_BYTE_Buffer[Key_To_LEDNumber[current_colum][current_row]][1] =
                LED_BYTE_Buffer[Key_To_LEDNumber[current_colum][current_row]][2] = LED_BRIGHTNESS;
            }
            KEYBOARD_data_ready = 1; // 产生事件
            if (KeyArr_Ptr[current_colum][current_row] == KEY_Fn) {  // 功能键
                Fn_state = 1;
            } else if (KeyArr_Ptr[current_colum][current_row] >= KEY_MouseL) {    // 鼠标操作
                PS2dat->data[0] |= 1 << KeyArr_Ptr[current_colum][current_row] - KEY_MouseL;
                KEYBOARD_mouse_ready = 1;
            } else if (KeyArr_Ptr[current_colum][current_row] >= KEY_LeftCTRL) {    // 特殊键
                Keyboarddat->data[0] |= 1 << (KeyArr_Ptr[current_colum][current_row] - KEY_LeftCTRL);
            } else {
                if (CustomKey[current_colum][current_row] == KEY_CapsLock) {  // 长按大小写键进入Extra_CustomKey层
                    KeyArr_ChangeTimes = 1; // 键盘布局转换-开始计数
                } else {
                    Keyboarddat->data[KEYBOARD_data_index++] = KeyArr_Ptr[current_colum][current_row];
                    press_NormalKey = KeyArr_ChangeTimes > MAX_CHANGETIMES;
                }
            }
        } else if (KeyMatrix[current_colum][current_row] != 0 && Colum_GPIO_(ReadPortPin)( Colum_Pin[current_colum] ) != 0) {   // 弹起
            KeyMatrix[current_colum][current_row] = 0;
            KEYBOARD_data_ready = 1; // 产生事件
            if (KeyArr_Ptr[current_colum][current_row] == KEY_Fn) {  // 功能键
                Fn_state = 0;
            } else if (KeyArr_Ptr[current_colum][current_row] >= KEY_MouseL) {    // 鼠标操作
                PS2dat->data[0] &= ~(1 << KeyArr_Ptr[current_colum][current_row] - KEY_MouseL);
                KEYBOARD_mouse_ready = 1;
            } else if (KeyArr_Ptr[current_colum][current_row] >= KEY_LeftCTRL) {    // 特殊键
                Keyboarddat->data[0] &= ~(1 << (KeyArr_Ptr[current_colum][current_row] - KEY_LeftCTRL));
            } else {
                if (CustomKey[current_colum][current_row] == KEY_CapsLock) {  // 弹起大小写键离开Extra_CustomKey层
                    if (KeyArr_ChangeTimes > MAX_CHANGETIMES) {
                        KeyArr_Ptr = CustomKey;
                        OLED_ShowString(2, 1, "L1");
                    }
                    if (press_NormalKey == FALSE || KeyArr_ChangeTimes <= MAX_CHANGETIMES) {
                        if (KEYBOARD_data_index < 8) {
                            press_Capslock = TRUE;
                            Keyboarddat->data[KEYBOARD_data_index++] = KEY_CapsLock;
                        }
                    }
                    KeyArr_ChangeTimes = 0;
                    press_NormalKey = FALSE;
                } else {
                    for (key_idx = 2; key_idx < 8; key_idx++) {
                        if (Keyboarddat->data[key_idx] == CustomKey[current_colum][current_row] ||
                            Keyboarddat->data[key_idx] == Extra_CustomKey[current_colum][current_row]) {  // 弹起按键2层都清除
                            memcpy(&Keyboarddat->data[key_idx], &Keyboarddat->data[key_idx] + 1, 7 - key_idx);
                            Keyboarddat->Key6 = 0;
                        }
                    }
                    KEYBOARD_data_index--;
                }
            }
        }
    }
    Row_GPIO_(SetBits)( Row_Pin[current_row++] );
    if (current_row >= ROW_SIZE) {
        current_row = 0;
    }
    Row_GPIO_(ResetBits)( Row_Pin[current_row] );
}

/*******************************************************************************
* Function Name  : KEYBOARD_EnterPasskey
* Description    : 键盘输入配对码
* Input          : 指向配对码值的指针
* Return         : 返回0表示输入完成
*******************************************************************************/
uint8_t KEYBOARD_EnterPasskey( uint32_t* key )
{
  static uint8_t idx = 0;
  static uint32_t passkey = 0;
  static char passkey_str[7] = { '\0' };
  const uint8_t keyhash[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
  if ( Keyboarddat->Key1 == KEY_BACKSPACE ) { // 退格单独处理
      if ( idx > 0 ) {
          passkey_str[--idx] = '\0';
          passkey /= 10;
      }
      if ( passkey_str[0] == '\0' ) {
        OLED_PRINT("Passkey=?");
      } else {
        OLED_PRINT("%s", passkey_str);
      }
  } else if ( idx == 6 ) {  // 最后一个按键是Enter则结束
      if ( Keyboarddat->Key1 == KEY_ENTER ) {
          OLED_PRINT("Send!");
          tmos_start_task( halTaskID, OLED_EVENT, MS1_TO_SYSTEM_TIME(3000) );
          *key = passkey;
          passkey = idx = passkey_str[0] = 0;
          return 0;
      }
  } else if ( Keyboarddat->Key1 >= KEY_1 && Keyboarddat->Key1 <= KEY_0 ){
      passkey = passkey * 10 + keyhash[Keyboarddat->Key1 - KEY_1];
      passkey_str[idx++] = keyhash[Keyboarddat->Key1 - KEY_1] + 0x30;
      passkey_str[idx] = '\0';
      OLED_PRINT("%s", passkey_str);
  }
  return 1;
}

