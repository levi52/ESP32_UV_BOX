#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "ArduinoZlib.h"
#include <WebServer.h>
#include <esp_wifi.h>     // 用于esp_wifi_restore() 删除保存的wifi信息
#include <Preferences.h>

extern int currentWeek,currentHour,currentMinute,currentSecond;
extern int currentDay,currentMonth,currentYear; 
extern String wk[7];
extern Preferences prefs; // 声明Preferences对象
typedef struct {
    String text; // 天气类型
    int icon; // 天气图标
    int temp; // 温度
    String feelsLike; // 体感温度
    int pressure; // 压强
    String windDir; // 风力
    String windScale; // 风力
    int vis;  // 能见度
    int humidity; // 湿度
    int air; // 
    String category; // 空气质量指数级别
    String pm10; // PM10
    String pm2p5; // PM2.5
    String no2; // 二氧化氮
    String so2; // 二氧化硫
    String co; // 一氧化碳
    String o3; // 臭氧
} NowWeather; // 定义结构体

typedef struct {
    String day0wea;
    int day0wea_img;
    String day0date;
    int day0tem_day;
    int day0tem_night;
    String day1wea;
    int day1wea_img;
    String day1date;
    int day1tem_day;
    int day1tem_night;
    String day2wea;
    int day2wea_img;
    String day2date;
    int day2tem_day;
    int day2tem_night;
    String day3wea;
    int day3wea_img;
    String day3date;
    int day3tem_day;
    int day3tem_night;
    String day4wea;
    int day4wea_img;
    String day4date;
    int day4tem_day;
    int day4tem_night;
    String day5wea;
    int day5wea_img;
    String day5date;
    int day5tem_day;
    int day5tem_night;
    String day6wea;
    int day6wea_img;
    String day6date;
    int day6tem_day;
    int day6tem_night;
} FutureWeather;
extern NowWeather nowWeather; // 记录查询到的实况天气数据
extern FutureWeather futureWeather; // 未来天气

extern int followingCount;
extern int fansCount;
extern int likes;
extern int videoView;

void time_init(void); // ntp
void get_time(void); // 获取当前时间

void get_weather_now(void); // 获取当前天气
void get_weather_future(void); // 获取未来天气
void get_air(void); // 获取空气质量
void get_city_id(void);
void get_biliFollow(void);
void get_biliView(void);
String urlEncode(const String& text);

bool wifi_check(void);
bool wifi_scan(void);
void wifi_connect(int timeOut_s);
void wifi_disconnect(void);
void ap_init(void);
void server_init(void);
void handleRoot(void);
void handleConfigWifi(void);
void handleNotFound(void);
void doClient(void);
void wifiConfigBySoftAP(void);
void set_info(void);
void get_info(void);
#endif