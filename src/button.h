/**
 * @file button.h
 * @brief 按键控制
 * @author levi5
 * @version 1.0
 * @date 2024-05-25
 * @copyright Copyright (c) 2024, levi5
 * @details
 * @par 修改日志
*/

#ifndef __BUTTON_H__
#define __BUTTON_H__
#include "OneButton.h"

#include "ui.h"
extern OneButton button1;
extern OneButton button2;
extern OneButton button3;

void button_init(void);
void button_tick(void);
void click1();
void doubleclick1();
void longPressStart1();
void longPress1();
void longPressStop1();
void click2();
void doubleclick2();
void longPressStart2();
void longPress2();
void longPressStop2();
void click3();
void doubleclick3();
void longPressStart3();
void longPress3();
void longPressStop3();
#endif