/********************************** (C) COPYRIGHT *******************************
* File Name          : snake.h
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2022/1/28
* Description        :
*******************************************************************************/

#ifndef SNAKE_H
  #define SNAKE_H

  #define MAX_SNAKE_LENGTH              30    // 最长蛇长
  #define KEYBOARD_SNAKE_ROWCNT         6     // 键盘行数 - 其它键盘布局需修改此处
  #define KEYBOARD_SNAKE_COLUMCNT       14    // 键盘列数 - 其它键盘布局需修改此处

  typedef enum {
    DirUp = 1,
    DirDown,
    DirLeft,
    DirRight,
  }SnakeDir;

  typedef struct {
    uint8_t PosX;
    uint8_t PosY;
  }SnakePos;

  SnakeDir BodyDir[MAX_SNAKE_LENGTH];

  void Snake_Init( void );
  void MoveSnake( void );

#endif

