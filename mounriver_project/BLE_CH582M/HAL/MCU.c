/********************************** (C) COPYRIGHT *******************************
 * File Name          : MCU.c
 * Author             : ChnMasterOG
 * Version            : V1.2
 * Date               : 2022/1/26
 * Description        : 硬件任务处理函数及BLE和硬件初始化
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CH58x_common.h"
#include "HAL.h"
#include <string.h>

/* 彩蛋 */
#include "snake.h"

tmosTaskID halTaskID=INVALID_TASK_ID;
BOOL connection_state[2] = { FALSE, FALSE };  // USB/BLE state
uint32_t EP_counter = 0;  // 彩蛋计数器

/*******************************************************************************
 * @fn          Lib_Calibration_LSI
 *
 * @brief       内部32k校准
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void Lib_Calibration_LSI( void )
{
  Calibration_LSI( Level_64 );
}

#if (defined (BLE_SNV)) && (BLE_SNV == TRUE)
/*******************************************************************************
 * @fn          Lib_Read_Flash
 *
 * @brief       Lib 操作Flash回调
 *
 * input parameters
 *
 * @param       addr.
 * @param       num.
 * @param       pBuf.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
u32 Lib_Read_Flash( u32 addr, u32 num, u32 *pBuf )
{
  EEPROM_READ( addr, pBuf, num * 4 );
  return 0;
}

/*******************************************************************************
 * @fn          Lib_Write_Flash
 *
 * @brief       Lib 操作Flash回调
 *
 * input parameters
 *
 * @param       addr.
 * @param       num.
 * @param       pBuf.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
u32 Lib_Write_Flash( u32 addr, u32 num, u32 *pBuf )
{
  EEPROM_ERASE( addr, EEPROM_PAGE_SIZE*2 );
  EEPROM_WRITE( addr, pBuf, num * 4 );
  return 0;
}
#endif

/*******************************************************************************
 * @fn          CH57X_BLEInit
 *
 * @brief       BLE 库初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void CH57X_BLEInit( void )
{
  uint8 i;
  bleConfig_t cfg;
  if ( tmos_memcmp( VER_LIB, VER_FILE, strlen( VER_FILE )  ) == FALSE )
  {
    PRINT( "head file error...\n" );
    while( 1 )
      ;
  }
  SysTick_Config( SysTick_LOAD_RELOAD_Msk );
  PFIC_DisableIRQ( SysTick_IRQn );

  tmos_memset( &cfg, 0, sizeof(bleConfig_t) );
  cfg.MEMAddr = ( u32 ) MEM_BUF;
  cfg.MEMLen = ( u32 ) BLE_MEMHEAP_SIZE;
  cfg.BufMaxLen = ( u32 ) BLE_BUFF_MAX_LEN;
  cfg.BufNumber = ( u32 ) BLE_BUFF_NUM;
  cfg.TxNumEvent = ( u32 ) BLE_TX_NUM_EVENT;
  cfg.TxPower = ( u32 ) BLE_TX_POWER;
#if (defined (BLE_SNV)) && (BLE_SNV == TRUE)
  FLASH_ROM_LOCK( 0 );                    // 解锁flash
  cfg.SNVAddr = ( u32 ) BLE_SNV_ADDR;
  cfg.readFlashCB = Lib_Read_Flash;
  cfg.writeFlashCB = Lib_Write_Flash;
#endif
#if( CLK_OSC32K )	
  cfg.SelRTCClock = ( u32 ) CLK_OSC32K;
#endif
  cfg.ConnectNumber = ( PERIPHERAL_MAX_CONNECTION & 3 ) | ( CENTRAL_MAX_CONNECTION << 2 );
  cfg.srandCB = SYS_GetSysTickCnt;
#if (defined TEM_SAMPLE)  && (TEM_SAMPLE == TRUE)
  cfg.tsCB = HAL_GetInterTempValue;    // 根据温度变化校准RF和内部RC( 大于7摄氏度 )
#if( CLK_OSC32K )
  cfg.rcCB = Lib_Calibration_LSI;    // 内部32K时钟校准
#endif
#endif
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
  cfg.sleepCB = CH58X_LowPower;    // 启用睡眠
#endif
#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
  for ( i = 0; i < 6; i++ )
    cfg.MacAddr[i] = MacAddr[5 - i];
#else
  {
    uint8 MacAddr[6];
    GetMACAddress( MacAddr );
    for(i=0;i<6;i++) cfg.MacAddr[i] = MacAddr[i];    // 使用芯片mac地址
  }
#endif
  if ( !cfg.MEMAddr || cfg.MEMLen < 4 * 1024 )
    while( 1 )
      ;
  i = BLE_LibInit( &cfg );
  if ( i )
  {
    PRINT( "LIB init error code: %x ...\n", i );
    while( 1 )
      ;
  }
}

/*******************************************************************************
 * @fn          HAL_ProcessEvent
 *
 * @brief       硬件层事务处理
 *
 * input parameters
 *
 * @param       task_id.
 * @param       events.
 *
 * output parameters
 *
 * @param       events.
 *
 * @return      None.
 */
tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  uint8 * msgPtr;

  if ( events & SYS_EVENT_MSG )
  {    // 处理HAL层消息，调用tmos_msg_receive读取消息，处理完成后删除消息。
    msgPtr = tmos_msg_receive( task_id );
    if ( msgPtr )
    {
      /* De-allocate */
      tmos_msg_deallocate( msgPtr );
    }
    return events ^ SYS_EVENT_MSG;
  }

  // 板载LED闪烁事件
  if ( events & LED_BLINK_EVENT )
  {
#if (defined HAL_LED) && (HAL_LED == TRUE)
    HalLedUpdate( );
#endif // HAL_LED
    return events ^ LED_BLINK_EVENT;
  }

  // 板载按键事件
  if ( events & KEY_EVENT )
  {
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
    HAL_KeyPoll(); /* Check for keys */
    tmos_start_task( halTaskID, KEY_EVENT, MS1_TO_SYSTEM_TIME(100) );
#endif
    return events ^ KEY_EVENT;
  }

  // 电池ADC检测事件 - 使能DMA
  if ( events & BATTERY_EVENT )
  {
    BATTERY_ADC_Calculation( );
    BATTERY_DrawBMP( );
    BATTERY_DMA_ENABLE( );
    tmos_start_task( halTaskID, BATTERY_EVENT, MS1_TO_SYSTEM_TIME(1000) );  // 1s更新采样值
  }

  // 定时主循环事件
  if ( events & MAIN_CIRCULATION_EVENT)
  {
    /* 专属彩蛋模式 */
    if ( PaintedEggMode == TRUE ) {
      KEYBOARD_detection();
      if (KEYBOARD_data_ready != 0) {
        KEYBOARD_data_ready = 0;
        if (KEYBOARD_Custom_Function() != 0) {
          switch (Keyboarddat->Key1) {
            case KEY_W: BodyDir[0] = DirUp; break;
            case KEY_S: BodyDir[0] = DirDown; break;
            case KEY_A: BodyDir[0] = DirLeft; break;
            case KEY_D: BodyDir[0] = DirRight; break;
          }
        }
      }
      if (++EP_counter > 50) {  // 50次事件更新一次
        EP_counter = 0;
        MoveSnake();
      }
      tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, MS1_TO_SYSTEM_TIME(10) ); // 10ms周期
      return events ^ MAIN_CIRCULATION_EVENT;
    }
    /**********/
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
    if (PS2_data_ready != 0) {    // 发送小红点鼠标数据
        PS2_data_ready = 0;
        if (PS2_byte_cnt == 3) {  // 接收完数据报
            PS2_byte_cnt = 0;
            HIDMouse[2] = -HIDMouse[2]; // 反转Y轴
            if (USB_Ready == TRUE) {
                tmos_start_task( usbTaskID, USB_MOUSE_EVENT, 2 );  //USB鼠标事件
            } else if (BLE_Ready == TRUE) {
                tmos_start_task( hidEmuTaskId, START_MOUSE_REPORT_EVT, MS1_TO_SYSTEM_TIME(2) );  //蓝牙鼠标事件 2ms处理
            }
        }
        PS2_En_Data_Report();
    }
#endif
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
    KEYBOARD_detection();
    if (KEYBOARD_data_ready != 0) {    // 发送键盘数据
        KEYBOARD_data_ready = 0;
        if ( KEYBOARD_Custom_Function() ) { // 带有Fn键处理信息则不产生键盘事件
          if ( USB_Ready == TRUE ) {
              tmos_start_task( usbTaskID, USB_KEYBOARD_EVENT, 2 );  // USB键盘事件
          } else if (BLE_Ready == TRUE) {
              tmos_start_task( hidEmuTaskId, START_KEYBOARD_REPORT_EVT, MS1_TO_SYSTEM_TIME(2) );  // 蓝牙键盘事件 2ms处理
          }
        }
        if (KEYBOARD_mouse_ready != 0) { // 发送键盘鼠标数据
            KEYBOARD_mouse_ready = 0;
            tmos_memset(&HIDMouse[1], 0, 3);   // 只按左中右键没有其他操作
            if (USB_Ready == TRUE) {
                tmos_start_task( usbTaskID, USB_MOUSE_EVENT, 2 );  //USB鼠标事件
            } else if (BLE_Ready == TRUE) {
                tmos_start_task( hidEmuTaskId, START_MOUSE_REPORT_EVT, MS1_TO_SYSTEM_TIME(2) );  //蓝牙鼠标事件 2ms处理
            }
        }
    }
#endif
    // 检测USB/蓝牙连接状态：连接状态改变OLED显示
    if (connection_state[0] != USB_Ready) {
      connection_state[0] = USB_Ready;
      OLED_PRINT("USB: %d, BLE: %d", USB_Ready, BLE_Ready);
    } else if (connection_state[1] != BLE_Ready) {
      connection_state[1] = BLE_Ready;
      OLED_PRINT("USB: %d, BLE: %d", USB_Ready, BLE_Ready);
    }
    tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, MS1_TO_SYSTEM_TIME(10) ); // 10ms周期
    return events ^ MAIN_CIRCULATION_EVENT;
  }

  if ( events & WS2812_EVENT )
  {
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
    if (USB_Ready == TRUE) {
      WS2812_Send( );
    }
    tmos_start_task( halTaskID, WS2812_EVENT, MS1_TO_SYSTEM_TIME(60) ); // 60ms周期控制背光
#endif
    return events ^ WS2812_EVENT;
  }

  // 硬件初始化事件
  if ( events & HAL_REG_INIT_EVENT )
  {
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)	// 校准任务，单次校准耗时小于10ms
    BLE_RegInit();    // 校准RF
#if( CLK_OSC32K )	
    Lib_Calibration_LSI();    // 校准内部RC
#endif
    tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );
    return events ^ HAL_REG_INIT_EVENT;
#endif
  }

  // 测试事件
  if ( events & HAL_TEST_EVENT )
  {
    PRINT( "*\n" );
    tmos_start_task( halTaskID, HAL_TEST_EVENT, MS1_TO_SYSTEM_TIME( 1000 ) );
    return events ^ HAL_TEST_EVENT;
  }

  return 0;
}

/*******************************************************************************
 * @fn          HAL_Init
 *
 * @brief       硬件初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HAL_Init()
{
  char debug_info[520] = "All Ready!";
  halTaskID = TMOS_ProcessEventRegister( HAL_ProcessEvent );
  HAL_TimeInit();
#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE)
  HAL_SleepInit();
#endif
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HAL_KeyInit( );
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
  HAL_OLED_Init( );
#endif
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
  BATTERY_Init( );
#endif
#if (defined HAL_USB) && (HAL_USB == TRUE)
  HAL_USBInit( );
#endif
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
  PS2_Init(debug_info, TRUE); // PS/2中断实现
#endif
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  KEYBOARD_Init( );
#endif
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
  WS2812_PWM_Init( );
#endif
#if (defined HAL_LED) && (HAL_LED == TRUE)
  debug_info[7] = '\0';
  if ( strcmp(debug_info, "[ERROR]") == 0 ) {
    HAL_LedInit(1);
  } else {
    HAL_LedInit(0);
  }
#endif
#if ( defined BLE_CALIBRATION_ENABLE ) && ( BLE_CALIBRATION_ENABLE == TRUE )
  tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );    // 添加校准任务，单次校准耗时小于10ms
#endif
PRINT("%s\n", debug_info);
OLED_PRINT("%s", debug_info);
//  tmos_start_task( halTaskID, HAL_TEST_EVENT, 1600 );    // 添加测试任务
}

/*******************************************************************************
 * @fn          HAL_GetInterTempValue
 *
 * @brief       如果使用了ADC中断采样，需在此函数中暂时屏蔽中断.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
uint16 HAL_GetInterTempValue( void )
{
  uint8 sensor, channel, config, tkey_cfg;
  uint16 adc_data;
  
  tkey_cfg = R8_TKEY_CFG;
  sensor = R8_TEM_SENSOR;
  channel = R8_ADC_CHANNEL;
  config = R8_ADC_CFG;
  ADC_InterTSSampInit();
  R8_ADC_CONVERT |= RB_ADC_START;
  while( R8_ADC_CONVERT & RB_ADC_START )
    ;
  adc_data = R16_ADC_DATA;
  R8_TEM_SENSOR = sensor;
  R8_ADC_CHANNEL = channel;
  R8_ADC_CFG = config;
  R8_TKEY_CFG = tkey_cfg;
  return ( adc_data );
}

/******************************** endfile @ mcu ******************************/
