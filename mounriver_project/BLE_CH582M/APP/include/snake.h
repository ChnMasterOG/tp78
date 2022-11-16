/********************************** (C) COPYRIGHT *******************************
* File Name          : snake.h
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2022/1/28
* Description        :
* SPDX-License-Identifier: GPL-3.0
*******************************************************************************/

#ifndef SNAKE_H
  #define SNAKE_H

  #define MAX_SNAKE_LENGTH              30    // ×î³¤Éß³¤

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

