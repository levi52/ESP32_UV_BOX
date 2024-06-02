/**
 * @file main.cpp
 * @brief ESP32 UV BOX
 * @author levi5
 * @version 1.0
 * @date 2024-05-20
 * @copyright Copyright (c) 2024, levi5
 * @details
 * @par 修改日志
 * @link 
 * bilibili: https://space.bilibili.com/378576508
 * oshwhub: 
 * github: https://github.com/levi52
 * blog: https://levi52.github.io/
 * csdn: 
*/
#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>

#include "sensor.h"
#include "button.h"
#include "ui.h"
#include "network.h"

// 任务
void aht20_bmp280_task(void *pt);
void weather_task(void *pt);
void adc_task(void *pt);
void time_task(void *pt);
void ui_task(void *pt);
void init_ui_task(void *pt);

void setup() {
  // 串口初始化
  Serial.begin(115200);
  
  // 读取数据
  get_info();

  // I2C设备初始化
  Wire.begin(21, 22);
  sensor_aht20_init();
  vTaskDelay(500);
  sensor_bmp280_init();

  // ADC采集初始化
  sensor_uv_init();
  sensor_battery_init();

  // 其他初始化
  button_init();
  light_init();

  // 屏幕初始化
  set_back_brightness(brightness);
  screen_init();
  // INIT_UI();
  xTaskCreatePinnedToCore(init_ui_task, "init_ui_task", 1024 * 6, NULL, 2, NULL, 0);
  // 网络初始化
  wifi_connect(15);
  if(WiFi.status() == WL_CONNECTED)
  {
    time_init();
    get_time();
  }
  aht20Data = sensor_aht20_data();
  vTaskDelay(500);
  bmp280Data = sensor_bmp280_data();
  uvLevel = sensor_uv_data();
  batteryVoltage = sensor_battery_data();
  vTaskDelay(500);
  // 创建任务
  xTaskCreatePinnedToCore(aht20_bmp280_task, "aht20_bmp280_task", 1024 * 2, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(adc_task, "adc_task", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(weather_task, "weather update", 1024 * 9, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(time_task, "time update", 1024 * 2, NULL, 1, NULL, 0);
}

void loop() {
  BOX_UI();
  button_tick();
  if(WiFi.status() != WL_CONNECTED)
  {
    doClient(); 
  }
}

void init_ui_task(void *pt)
{
  INIT_UI();
  vTaskDelete(NULL);
}
/**
 * @name weather_task
 * @brief 获取天气信息
 * @param pt 
 */
void weather_task(void *pt)
{
  while(1)
  {
      if(WiFi.status() == WL_CONNECTED)
      {
        get_weather_now();
        vTaskDelay(1000);
        get_weather_future();
        vTaskDelay(1000);
        get_air();
        vTaskDelay(1000);
        get_biliFollow();
      }
      vTaskDelay(1000*60*5);
  }
}

/**
 * @name aht20_bmp280_task
 * @brief 获取温湿度、气压传感器数据
 * @param pt
*/
void aht20_bmp280_task(void *pt)
{
  while (1)
  {
    aht20Data = sensor_aht20_data();
    vTaskDelay(1000);
    bmp280Data = sensor_bmp280_data();
    vTaskDelay(1000 * 4);
  }
}

/**
 * @name adc_task
 * @brief UV ADC采集
 * @param pt
*/
void adc_task(void *pt)
{
  while (1)
  {
    uvLevel = sensor_uv_data();
    batteryVoltage = sensor_battery_data();
    vTaskDelay(1000 * 3);
  }
}

/**
 * @name time_task
 * @brief NTP获取时间
 * @param pt
*/
void time_task(void *pt)
{
  while(1)
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      get_time();
    }
    vTaskDelay(1000);
  }
}