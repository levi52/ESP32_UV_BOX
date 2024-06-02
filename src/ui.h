/**
 * @file ui.h
 * @brief UI界面
 * @author levi5
 * @version 1.0
 * @date 2024-05-25
 * @copyright Copyright (c) 2024, levi5
 * @details
 * @par 修改日志
*/

#ifndef __UI_H__
#define __UI_H__

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <FastLED.h>

#include "sensor.h"
#include "network.h"

extern AHT20Data aht20Data;
extern BMP280Data bmp280Data;
extern uint8_t uvLevel;
extern float batteryVoltage;
extern bool pushFlag;
extern bool enterFlag;

extern int8_t currentPage; // 当前页面 0: 首页 1: 传感器 2: 网络 3: 灯光 4: 设置
extern int8_t currentIndex; // 0: 1级 1: 2级 2: 3级
extern int8_t menuIndex; // 主菜单页索引 0：sensor 1：network 2：led 3：setting 
extern int8_t sensorIndex; // 传感器页索引 0：UV 1：AHT20 2：BMP280
extern int8_t networkIndex; // 网络页索引 0: clock 1: bilibili 2:forecast 3: airq
extern int8_t settingIndex; // 设置页索引 0: set
extern int8_t ledIndex; // 灯光页索引 0: led
extern int8_t lightIndex;

#include "img/icon_logo.h"
// 首页图标
#include "img/icon_sensor.h"
#include "img/icon_network.h"
#include "img/icon_led.h"
#include "img/icon_setting.h"

// SENSOR图标
#include "img/icon_uv.h"
#include "img/icon_temp_humi.h"
#include "img/icon_pressure.h"
#include "img/icon_alti.h"
#include "img/icon_humi.h"
#include "img/icon_temp.h"
#include "img/icon_press.h"

// NETWORK图标
#include "img/icon_time.h"
#include "img/icon_forecast.h"
#include "img/icon_aiq.h"
#include "img/icon_bilibili.h"
#include "img/icon_humi1.h"
#include "img/icon_temp1.h"
#include "img/icon_bili.h"

// SETTING 图标
#include "img/icon_wifi.h"
#include "img/icon_brightness.h"
#include "img/icon_bluetooth.h"
#include "img/icon_about.h"
#include "img/icon_head.h"

// WEATHER 图标
#include "img/icon_sunny.h"
#include "img/icon_cloudy.h"
#include "img/icon_light_rain.h"
#include "img/icon_overcast.h"
#include "img/icon_shower_rain.h"
#include "img/icon_thundershower.h"
#include "img/icon_heavy_rain.h"
#include "img/icon_light_snow.h"
#include "img/icon_foggy.h"
#include "img/icon_haze.h"
#include "img/icon_null.h"
#include "img/icon_sandstorm.h"
#include "img/icon_sand.h"

// 字体
#include "font/CCH20.h" // 字魂59号-创粗黑
#include "font/BoxedRegular18.h" // Boxed Regular
#include "font/AlimamaShuHeiTi25.h" // Boxed Regular
#include "font/BoxedRegular35.h" // Boxed Regular
#include "font/AlimamaShuHeiTi16.h" // AlimamaShuHeiTi
#include "font/DigitalNumbers30.h" // DigitalNumbers

extern int16_t brightness;
extern bool brightness_page;
void INIT_UI(void);
void BOX_UI(void);
void SENSOR_UI(void);
void NETWORK_UI(void);
void LIGHT_UI(void);
void SETTING_UI(void);

void AHT20_UI(void);
void BMP280_UI(void);
void UV_UI(void);

void CLOCK_UI(void);
void AIQ_UI(void);
void FORECAST_UI(void);
void BILIBILI_UI(void);

void BRIGHTNESS_UI(void);
void WIFI_UI(void);
void BLUETOOTH_UI(void);
void ABOUT_UI(void);

void light_init(void);
void light_close(void);

void screen_init(void);
void set_back_brightness(uint8_t brightness);
void set_weather_icon(int icon);

String num2str(int digits);

#endif