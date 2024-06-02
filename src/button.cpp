/**
 * @file button.cpp
 * @brief 按键控制
 * @author levi5
 * @version 1.0
 * @date 2024-05-25
 * @copyright Copyright (c) 2024, levi5
 * @details
 * @par 修改日志
*/

#include "button.h"
// 拨轮开关
#define BTN1 27 // prev
#define BTN2 25 // next
#define BTN3 26 // enter

OneButton button1(BTN1, false, false);
OneButton button2(BTN2, false, false);
OneButton button3(BTN3, false, false);

/**
 * @name button_init
 * @brief 按键初始化
 * @author levi5
*/
void button_init(void)
{
  button1.attachClick(click1);
  button1.attachDoubleClick(doubleclick1);
  button1.attachLongPressStart(longPressStart1);
  button1.attachLongPressStop(longPressStop1);
  button1.attachDuringLongPress(longPress1);

  button2.attachClick(click2);
  button2.attachDoubleClick(doubleclick2);
  button2.attachLongPressStart(longPressStart2);
  button2.attachLongPressStop(longPressStop2);
  button2.attachDuringLongPress(longPress2);

  button3.attachClick(click3);
  button3.attachDoubleClick(doubleclick3);
  button3.attachLongPressStart(longPressStart3);
  button3.attachLongPressStop(longPressStop3);
  button3.attachDuringLongPress(longPress3);
}

/**
 * @name button_tick
 * @brief 按键定时器
 * @author levi5
*/
void button_tick(void)
{
  button1.tick();
  button2.tick();
  button3.tick();
}

void click1()
{
  Serial.println("Button 1 click. prev");
  pushFlag = true;
  if(currentIndex != 2)
  {
    switch (currentPage)
    {
      case 0:
        /* code */
        menuIndex--;
        if(menuIndex < 0)
        {
          menuIndex = 3;
        }
        break;
      case 1:
        /* code */
        sensorIndex--;
        if(sensorIndex < 0)
        {
          sensorIndex = 2;
        }
        break;
      case 2:
        /* code */
        networkIndex--;
        if(networkIndex < 0)
        {
          networkIndex = 3;
        }
        break;
      case 3:
        /* code */
        // ledIndex--;
        // if(ledIndex < 0)
        // {
        //   ledIndex = 0;
        // }
        lightIndex--;
        if(lightIndex < 0)
        {
          lightIndex = 6;
        }
        break;
      case 4:
        /* code */
        settingIndex--;
        if(settingIndex < 0)
        {
          settingIndex = 3;
        }
        break;
      default:
        break;
    }
  }
  else if(currentIndex == 2)
  {
    if(brightness_page)
    {
      brightness -= 24;
      if(brightness < 10)
      {
        brightness = 10;
      }
    }
  }
}  // click1

// This function will be called when the button1 was pressed 2 times in a short timeframe.
void doubleclick1() 
{
  Serial.println("Button 1 doubleclick.");
}  // doubleclick1


// This function will be called once, when the button1 is pressed for a long time.
void longPressStart1() 
{
  Serial.println("Button 1 longPress start");
}  // longPressStart1

// This function will be called often, while the button1 is pressed for a long time.
void longPress1() 
{
  Serial.println("Button 1 longPress...");
}  // longPress1

// This function will be called once, when the button1 is released after beeing pressed for a long time.
void longPressStop1() 
{
  Serial.println("Button 1 longPress stop");
}  // longPressStop1

void click2() {
  Serial.println("Button 2 click. next");
  pushFlag = true;
  if(currentIndex != 2)
  {
    switch (currentPage)
    {
      case 0:
        /* code */
        menuIndex++;
        if(menuIndex > 3)
        {
          menuIndex = 0;
        }
        break;
      case 1:
        /* code */
        sensorIndex++;
        if(sensorIndex > 2)
        {
          sensorIndex = 0;
        }
        break;
      case 2:
        /* code */
        networkIndex++;
        if(networkIndex > 3)
        {
          networkIndex = 0;
        }
        break;
      case 3:
        /* code */
        // ledIndex++;
        // if(ledIndex > 0)
        // {
        //   ledIndex = 0;
        // }
        lightIndex++;
        if(lightIndex > 6)
        {
          lightIndex = 0;
        }
        break;
      case 4:
        /* code */
        settingIndex++;
        if(settingIndex > 3)
        {
          settingIndex = 0;
        }
        break;
      default:
        break;
    }
  }
  else if(currentIndex == 2)
  {
    if(brightness_page)
    {
      brightness += 24;
      if(brightness > 250)
      {
        brightness = 250;
      }
    }
  }
}  // click2

void doubleclick2() {
  Serial.println("Button 2 doubleclick.");
}  // doubleclick2

void longPressStart2() {
  Serial.println("Button 2 longPress start");
}  // longPressStart2

void longPress2() {
  Serial.println("Button 2 longPress...");
}  // longPress2
void longPressStop2() {
  Serial.println("Button 2 longPress stop");
}  // longPressStop2

void click3() {
  Serial.println("Button 3 click. enter");
  currentIndex++;
  if(currentIndex > 2)
  {
    currentIndex = 2;
  }
  enterFlag = true;
  pushFlag = true;
  Serial.println(currentIndex);
}  // click3

void doubleclick3() {
  Serial.println("Button 3 doubleclick.");
}  // doubleclick3

void longPressStart3() {
  Serial.println("Button 3 longPress start");
}  // longPressStart3

void longPress3() {
  Serial.println("Button 3 longPress...");
  if(currentPage == 3)
  {
    light_close();
  }
  // currentIndex--;
  // if(currentIndex < 0)
  // {
  //   currentIndex = 2;
  // }
  // enterFlag = false;
}  // longPress3

void longPressStop3() {
  Serial.println("Button 3 longPress stop");
  if(currentPage == 4 && currentIndex == 2 && settingIndex == 0)
  {
    brightness_page = false;
    set_info();
  }
  currentIndex--;
  if(currentIndex < 0)
  {
    currentIndex = 0;
  }
  Serial.println(pushFlag);
  enterFlag = false;
  pushFlag = true;
  Serial.println(pushFlag);
  Serial.println(currentIndex);
}  // longPressStop3