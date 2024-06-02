/**
 * @file ui.cpp
 * @brief UI界面
 * @author levi5
 * @version 1.0
 * @date 2024-05-25
 * @copyright Copyright (c) 2024, levi5
 * @details
 * @par 修改日志
*/

#include "ui.h"

// 屏幕引脚配置见 USER_SETUP.h
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

// SENSOR数据
AHT20Data aht20Data; // AHT20数据
BMP280Data bmp280Data; // BMP280数据
uint8_t uvLevel; // 紫外线强度
float batteryVoltage; // 电池电压

// WS2812参数
#define NUM_LEDS 1 // 灯珠数
#define DATA_PIN 32 // 数据引脚
CRGB leds[NUM_LEDS]; // 灯珠数组

// 背光参数
#define FREQ_BLK 2000 // 频率
#define CHANNEL_BLK 0 // 通道
#define RESOLUTION_BLK 8 // 分辨率
#define BLK_PIN 17 // LED背光引脚
int16_t brightness = 10;
// 首页图标
const uint16_t* main_icon[] = {
    icon_sensor,
    icon_network,
    icon_led,
    icon_setting
};

// 传感器页图标
const uint16_t* sensor_icon[] = {
    icon_uv,
    icon_temp_humi,
    icon_pressure
};

// 网络页图标
const uint16_t* network_icon[] = {
    icon_time,
    icon_bilibili,
    icon_forecast,
    icon_aiq
};

// 设置页图标
const uint16_t* setting_icon[] = {
    icon_brightness,
    icon_wifi,
    icon_bluetooth,
    icon_about
};
bool brightness_page = false;
int8_t currentPage; // 当前页面 0: 首页 1: 传感器 2: 网络 3: 灯光 4: 设置 5: UV 6: AHT20 7: BMP280 8: CLOCK 9: BILIBILI 10: FORECAST 11: AIQ 12: BRIGHHTNESS 13: WIFI 14: BLUETOOTH
int8_t currentIndex; // 0: 1级 1: 2级 2: 3级
int8_t menuIndex; // 主菜单页索引 0：sensor 1：network 2：led 3：setting 
int8_t sensorIndex; // 传感器页索引 0：UV 1：AHT20 2：BMP280
int8_t networkIndex; // 网络页索引 0: clock 1: bilibili 2:forecast 3: airq
int8_t settingIndex; // 设置页索引 0: set
int8_t ledIndex; // 灯光页索引 0: led
int8_t lightIndex; // 灯光颜色索引
uint8_t bar = 0;
bool pushFlag = true; // 刷新标志
bool enterFlag = false; // 确认标志
String menuNames[4] = {"SENSOR", "NETWORK", "LIGHT", "SETTING"}; // 主菜单名称
String sensorNames[3] = {"UV SD", "AHT20", "BMP280"};   // 传感器页名称
String networkNames[4] = {"CLOCK", "BILIBILI", "FORECAST", "AIR Q"};    // 网络页名称
String settingNames[4] = {"BRIGHTNESS", "WIFI", "BLUETOOTH", "ABOUT"};   // 设置页名称
String ledNames[1] = {"LIGHT"}; // 灯光页名称
uint32_t lightColor[7] = {0xb000, 0xf340, 0xf760, 0x27c0, 0x069e, 0xa9bf, 0xf9bd};  // 灯光颜色
CRGB lightColorRGB[7] = {CRGB(181, 0, 0), CRGB(247, 105, 0), CRGB(247, 239, 0), CRGB(33, 251, 0), CRGB(0, 210, 247), CRGB(173, 53, 255), CRGB(255, 53, 239)};   // 灯光颜色

/**
 * @name light_init
 * @brief 初始化并设置LED灯
 * 该函数负责初始化LED灯 strip，并设置其亮度。它使用FastLED库来控制LED灯。
 * @author levi5
 * @param 无
 * @return 无
 */
void light_init(void)
{
    // 初始化LED灯，指定LED类型为NEOPIXEL，数据线引脚为DATA_PIN，假设LED的顺序为GRB
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  
    // 设置LED灯的亮度为200（介于0和255之间）
    FastLED.setBrightness(200);
}

/**
 * @name light_close
 * @brief 关闭灯光
 * @author levi5
*/
void light_close(void)
{
    leds[0] = CRGB(0,0,0);
    FastLED.show();
}

/**
 * @name set_back_brightness
 * @brief 设置背光亮度
 * @author levi5
 * @param brightness 亮度 0-100
*/
void set_back_brightness(uint8_t brightness)
{
	ledcSetup(CHANNEL_BLK, FREQ_BLK, RESOLUTION_BLK);
	ledcAttachPin(BLK_PIN, CHANNEL_BLK);
	brightness = brightness >= 0 ? (brightness <= 250 ? brightness : 250) : 0;
	ledcWrite(CHANNEL_BLK, brightness);
}

/**
 * @name num2str
 * @brief 将整数转换为字符串
 * @author levi5
 * @param digits 整数数字
 * @return 返回包含数字的字符串。如果数字小于10，则在数字前添加一个0。
 */
String num2str(int digits)
{
    String s = ""; // 初始化为空字符串
    // 如果数字小于10，就在字符串前添加一个0
    if (digits < 10)
    s = s + "0";
    // 将整数转换为字符串并追加到s中
    s = s + digits;
    return s; // 返回最终构建的字符串
}

/**
 * @name screen_init
 * @brief 初始化屏幕
 * @author levi5
*/
void screen_init(void)
{
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
}

/**
 * @name INIT_UI
 * @brief 初始化UI
*/
void INIT_UI(void)
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.loadFont(BoxedRegular18);//10
    tft.pushImage(23, 0, 80, 80, icon_logo);
    tft.drawString("ESP32 UV BOX", 2, 90);
    tft.drawRoundRect(20, 120, 84, 10, 2, TFT_WHITE);
    uint8_t i;
    for(i=0; i<=84; i++)
    {
        tft.fillRoundRect(20, 120, i, 10, 2, 0xb3df);
        vTaskDelay(random(20, 35));
    }
    tft.fillScreen(TFT_BLACK);
}
/**
 * @name BOX_UI
 * @brief 首页UI
 * @author levi5
*/
void BOX_UI(void)
{
    if(currentIndex == 0) // 未确定 二级页面图标
    {
        if(pushFlag)
        {
            currentPage = 0; // 首页
            spr.createSprite(128, 160);
            spr.fillSprite(TFT_BLACK);
            spr.setTextDatum(TC_DATUM);
            spr.setTextSize(2);
            spr.setTextColor(TFT_CYAN);
            spr.drawString(menuNames[menuIndex], 64, 20); // 显示菜单名称
            spr.pushImage(23, 50, 80, 80, main_icon[menuIndex]); // 显示图标
            // 底部指示
            for (int i = 0; i < 4; i++)
            {
                uint16_t rectColor = (i == menuIndex) ? TFT_CYAN : TFT_WHITE; //指示颜色
                spr.fillRoundRect(i * 20 + 28, 140, 10, 5, 1, rectColor); // 28 48 68 88
            }
            spr.pushSprite(0, 0);
            spr.deleteSprite();
            pushFlag = false;
        }
    }
    else if(currentIndex == 1) // 确定 三级页面图标
    {
        if(pushFlag)
        {
            switch (menuIndex)
            {
                case 0:
                    currentPage = 1; // 传感器页
                    SENSOR_UI();
                    break;
                case 1:
                    currentPage = 2; // 网络页
                    NETWORK_UI();
                    break;
                case 2:
                    currentPage = 3; // 灯光页
                    LIGHT_UI();
                    break;
                case 3:
                    currentPage = 4; // 设置页
                    SETTING_UI();
                    break;
                default:
                    break;
            }
            pushFlag = false;
        }
    }
    else if(currentIndex == 2)
    {
        switch(currentPage)
        {
            case 1:
                if(sensorIndex == 0)
                {
                    UV_UI();
                }
                else if(sensorIndex == 1)
                {
                    AHT20_UI();
                }
                else if(sensorIndex == 2)
                {
                    BMP280_UI();
                }
                break;
            case 2:
                if(networkIndex == 0)
                {
                    CLOCK_UI();
                }
                else if(networkIndex == 1)
                {
                    BILIBILI_UI();
                }
                else if(networkIndex == 2)
                {
                    FORECAST_UI();
                }
                else if(networkIndex == 3)
                {
                    AIQ_UI();
                }
                break;
            case 4:
                if(settingIndex == 0)
                {
                    BRIGHTNESS_UI();
                    brightness_page = true;
                }
                else if(settingIndex == 1)
                {
                    WIFI_UI();
                }
                else if(settingIndex == 2)
                {
                    BLUETOOTH_UI();
                }
                else if(settingIndex == 3)
                {
                    ABOUT_UI();
                }
                break;
        }
    }
}

/**
 * @name SENSOR_UI
 * @brief 传感器UI
 * @author levi5
*/
void SENSOR_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(sensorNames[sensorIndex], 64, 20); // 显示传感器名称
    spr.pushImage(23, 50, 80, 80, sensor_icon[sensorIndex]); // 显示传感器图标
    // 底部指示
    for (int i = 0; i < 3; i++)
    {
        uint16_t circleColor = (i == sensorIndex) ? TFT_CYAN : TFT_WHITE; // 指示颜色
        spr.fillCircle(i * 20 + 43, 140, 2, circleColor); //43 63 83
    }
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * @name NETWORK_UI
 * @brief 网络UI
 * @author levi5
*/
void NETWORK_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(networkNames[networkIndex], 64, 20); // 网络功能名称
    spr.pushImage(23, 50, 80, 80, network_icon[networkIndex]); // 网络功能图标
    for (int i = 0; i < 4; i++)
    {
        uint16_t circleColor = (i == networkIndex) ? TFT_CYAN : TFT_WHITE; // 指示颜色
        spr.fillCircle(i * 20 + 28, 140, 2, circleColor);
    }
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * @name LIGHT_UI
 * @brief 灯光UI
 * @author levi5
*/
void LIGHT_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.pushImage(23, 30, 80, 80, icon_led);
    spr.pushSprite(0, 0);
    spr.deleteSprite();
    tft.fillCircle(63, 130, 8, lightColor[lightIndex]); // 灯光
    leds[0] = lightColorRGB[lightIndex]; // 灯光颜色
    FastLED.show();
}

/**
 * @name SETTING_UI
 * @brief 设置UI
 * @author levi5
*/
void SETTING_UI(void)
{
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_CYAN);
    spr.drawString(settingNames[settingIndex], 64, 20); // 显示传感器名称
    spr.pushImage(23, 50, 80, 80, setting_icon[settingIndex]); // 显示传感器图标
    // 底部指示
    for (int i = 0; i < 4; i++)
    {
        uint16_t circleColor = (i == settingIndex) ? TFT_CYAN : TFT_WHITE; // 指示颜色
        spr.fillCircle(i * 20 + 28, 140, 2, circleColor);
    }
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

/**
 * @name AHT20_UI
 * @brief AHT20 UI
 * @author levi5
*/
void AHT20_UI(void)
{
    if(pushFlag)
    {
        Serial.println(pushFlag);
        Serial.println("AHT20");
        tft.fillScreen(TFT_BLACK);

        tft.loadFont(CCH20);
        tft.pushImage(0, 25, 36, 36, icon_temp);
        tft.setTextColor(TFT_CYAN);
        tft.drawString("温度", 36, 30);

        tft.pushImage(0, 85, 36, 36, icon_humi);
        tft.drawString("湿度", 36, 90);
        tft.unloadFont();
        pushFlag = false;
    }
    // 温度数据
    spr.loadFont(BoxedRegular18);
    spr.createSprite(50, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(CC_DATUM);
    spr.drawFloat(aht20Data.temperature, 1, 25, 9);
    spr.pushSprite(78, 30);
    spr.deleteSprite();

    // 温度条
    spr.createSprite(120, 6);
    spr.drawRoundRect(0, 0, 90, 6, 2, TFT_WHITE);
    spr.fillRoundRect(0, 0, (int)(aht20Data.temperature * 0.9), 6, 2, 0xfb60);
    spr.pushSprite(36, 55);
    spr.deleteSprite();

    // 湿度数据
    spr.createSprite(50, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(CC_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawFloat(aht20Data.humidity, 1, 25, 9);
    spr.pushSprite(78, 90);
    spr.deleteSprite();
    spr.unloadFont();

    // 湿度条
    spr.createSprite(120, 6);
    spr.drawRoundRect(0, 0, 90, 6, 2, TFT_WHITE);
    spr.fillRoundRect(0, 0, (int)(aht20Data.humidity * 0.9), 6, 2, 0x3626);
    spr.pushSprite(36, 115);
    spr.deleteSprite();
}

/**
 * @name BMP280_UI
 * @brief BMP280 UI
 * @author levi5
*/
void BMP280_UI(void)
{
    if(pushFlag)
    {
        Serial.println(pushFlag);
        Serial.println("BMP280");
        tft.fillScreen(TFT_BLACK);

        tft.loadFont(CCH20);
        tft.pushImage(0, 20, 36, 36, icon_temp);
        tft.setTextColor(TFT_CYAN);
        tft.drawString("温度", 45, 20);

        tft.pushImage(0, 70, 36, 36, icon_alti);
        tft.drawString("海拔", 45, 70);

        tft.pushImage(0, 120, 36, 36, icon_press);
        tft.drawString("气压", 45, 120);
        tft.unloadFont();

        pushFlag = false;
    }

    // 温度数据
    spr.createSprite(80, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(ML_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawFloat(bmp280Data.temperature, 1, 0, 9);
    spr.unloadFont();
    spr.pushSprite(45, 42);
    spr.deleteSprite();

    // 海拔数据
    spr.createSprite(80, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(ML_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawFloat(bmp280Data.altitude, 1, 0, 9);
    spr.unloadFont();
    spr.pushSprite(45, 92);
    spr.deleteSprite();

    // 压强数据
    spr.createSprite(80, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(ML_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawFloat(bmp280Data.pressure / 100, 1, 0, 9);
    spr.unloadFont();
    spr.pushSprite(45, 142);
    spr.deleteSprite();
}

/**
 * @name UV_UI
 * @brief UV UI
 * @author levi5
*/
void UV_UI(void)
{
    if(pushFlag)
    {
        Serial.println("UV");
        Serial.println(pushFlag);
        Serial.println("UV");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.setTextSize(2);
        tft.drawString("UV SD", 35, 25);
        tft.drawString("Level: ", 10, 100);
        
        tft.fillCircle(15, 130, 8, 0x3626);
        tft.fillCircle(39, 130, 8, 0xffe0);
        tft.fillCircle(63, 130, 8, 0xfca0);
        tft.fillCircle(87, 130, 8, 0xf800);
        tft.fillCircle(111, 130, 8, 0xc4bf);

        pushFlag = false;
    }
    spr.createSprite(40, 18);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(ML_DATUM);
    spr.setTextColor(TFT_CYAN);
    spr.loadFont(BoxedRegular18);
    spr.drawString(num2str(uvLevel), 0, 9);
    spr.unloadFont();
    spr.pushSprite(85, 100);
    spr.deleteSprite();

    spr.createSprite(50, 30);
    // 等级颜色指示
    switch (uvLevel)
    {
        case 1:
        case 2:
            spr.fillRoundRect(0, 0, 50, 30, 5, 0x3626);
            break;
        case 3:
        case 4:
        case 5:
            spr.fillRoundRect(0, 0, 50, 30, 5, 0xffe0);
            break;
        case 6:
        case 7:
            spr.fillRoundRect(0, 0, 50, 30, 5, 0xfb60);
            break;
        case 8:
        case 9:
        case 10:
            spr.fillRoundRect(0, 0, 50, 30, 5, 0xf800);
            break;
        case 11:
        case 12:
            spr.fillRoundRect(0, 0, 50, 30, 5, 0xc4bf);
            break;
        default:
            spr.fillRoundRect(0, 0, 50, 30, 5, TFT_CYAN);
            break;
    }
    spr.pushSprite(38, 55);
    spr.deleteSprite();

    // 当前等级指示
    spr.createSprite(128, 6);
    switch (uvLevel)
    {
        case 1:
        case 2:
            spr.drawTriangle(15, 0, 10, 5, 20, 5, TFT_WHITE);
            break;
        case 3:
        case 4:
        case 5:
            spr.drawTriangle(39, 0, 34, 5, 44, 5, TFT_WHITE);
            break;
        case 6:
        case 7:
            spr.drawTriangle(63, 0, 58, 5, 68, 5, TFT_WHITE);
            break;
        case 8:
        case 9:
        case 10:
            spr.drawTriangle(87, 0, 82, 5, 92, 5, TFT_WHITE);
            break;
        case 11:
        case 12:
            spr.drawTriangle(111, 0, 106, 5, 116, 5, TFT_WHITE);
            break;
        default:
            break;
    }
    spr.pushSprite(0, 145);
    spr.deleteSprite();
}

/**
 * @name CLOCK_UI
 * @brief CLOCK_UI
 * @author levi5
*/
void CLOCK_UI(void)
{
    if(pushFlag)
    {
        Serial.println("CLOCK");
        Serial.println(pushFlag);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.pushImage(0, 85, 30, 30, icon_temp1);
        tft.pushImage(0, 125, 30, 30, icon_humi1);
        pushFlag = false;
    }
    spr.createSprite(128, 40);
    spr.fillSprite(TFT_BLACK);
    // spr.fillRoundRect(10, 0, 45, 32, 2, 0xc9a0);
    // spr.fillRoundRect(75, 0, 45, 32, 2, 0x0494);
    // spr.drawFastHLine(10, 16, 45,0xef7d);
    // spr.drawFastHLine(75, 16, 45,0xef7d);
    spr.loadFont(BoxedRegular35);
    spr.setTextDatum(CC_DATUM);
    spr.setTextColor(0xef7d);
    spr.drawString(num2str(currentHour) + " : " + num2str(currentMinute), 64, 20);//
    // spr.setTextDatum(ML_DATUM);
    // spr.drawString(num2str(currentHour), 7, 18);//num2str(currentHour) + " : " + num2str(currentMinute)
    // spr.setTextDatum(CC_DATUM);
    // spr.drawString(" : ", 63, 18);//num2str(currentHour) + " : " + num2str(currentMinute)
    // spr.setTextDatum(MR_DATUM);
    // spr.drawString(num2str(currentMinute), 120, 18);//num2str(currentHour) + " : " + num2str(currentMinute)
    spr.unloadFont();
    spr.pushSprite(0, 10);
    spr.deleteSprite();

    spr.loadFont(AlimamaShuHeiTi16);
    spr.createSprite(36, 18);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xfcc3);
    spr.setTextDatum(CC_DATUM);
    spr.drawString(wk[currentWeek], 18, 9);
    spr.pushSprite(7, 60);
    spr.deleteSprite();

    spr.createSprite(80, 18);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xf7ff);
    spr.setTextDatum(CC_DATUM);
    spr.drawString(num2str(currentMonth) + "月" + num2str(currentDay) + "日", 40, 9);
    spr.pushSprite(45, 60);
    spr.deleteSprite();

    spr.createSprite(45, 30);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xf7ff);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(num2str(nowWeather.temp), 0, 0);
    spr.drawRoundRect(0, 20, 45, 6, 1, 0xf7ff);
    spr.fillRoundRect(0, 20, (int)(nowWeather.temp * 0.45), 6, 1, 0xfb60);
    spr.pushSprite(30, 85);
    spr.deleteSprite();

    spr.createSprite(45, 30);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xf7ff);
    spr.setTextDatum(TL_DATUM);
    spr.drawString(num2str(nowWeather.humidity), 0, 0);
    spr.drawRoundRect(0, 20, 45, 6, 1, 0xf7ff);
    spr.fillRoundRect(0, 20, (int)(nowWeather.humidity * 0.45), 6, 1, 0x3626);
    spr.pushSprite(30, 125);
    spr.deleteSprite();

    spr.createSprite(50, 72);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xf7ff);
    spr.setTextDatum(TC_DATUM);
    spr.drawString(nowWeather.text, 25, 0);
    set_weather_icon(nowWeather.icon);
    spr.pushSprite(75, 85);
    spr.deleteSprite();
    spr.unloadFont();
}

/**
 * @name AIQ_UI
 * @brief AIQ_UI
 * @author levi5
*/
void AIQ_UI(void)
{
    if(pushFlag)
    {
        Serial.println("AIQ");
        Serial.println(pushFlag);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        pushFlag = false;
    }
    spr.createSprite(128, 160);
    spr.loadFont(AlimamaShuHeiTi25);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    
    spr.fillRoundRect(0, 2, 128, 25, 5, 0xfd20);
    spr.fillRoundRect(0, 28, 128, 25, 5, 0xfbea);
    spr.fillRoundRect(0, 54, 128, 25, 5, 0xfb50);
    spr.fillRoundRect(0, 80, 128, 25, 5, 0x36ae);
    spr.fillRoundRect(0, 106, 128, 25, 5, 0x751f);
    spr.fillRoundRect(0, 132, 128, 25, 5, 0x529d);

    spr.setTextColor(TFT_WHITE);
    spr.drawString("PM10", 5, 3);
    spr.drawString("PM2.5", 5, 29);
    spr.drawString("NO2", 5, 55);
    spr.drawString("SO2", 5, 81);
    spr.drawString("CO", 5, 107);
    spr.drawString("O3", 5, 133);

    spr.drawString(nowWeather.pm10, 82, 3);
    spr.drawString(nowWeather.pm2p5, 82, 29);
    spr.drawString(nowWeather.no2, 82, 55);
    spr.drawString(nowWeather.so2, 82, 81);
    spr.drawString(nowWeather.co, 82, 107);
    spr.drawString(nowWeather.o3, 82, 133);
    spr.pushSprite(0, 0);
    spr.deleteSprite();
    spr.unloadFont();
}

/**
 * @name FORECAST_UI
 * @brief FORECAST_UI
 * @author levi5
*/
void FORECAST_UI(void)
{
    if(pushFlag)
    {
        Serial.println("FORECAST");
        Serial.println(pushFlag);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        pushFlag = false;
    }
    spr.createSprite(128, 160);
    spr.loadFont(AlimamaShuHeiTi25);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    
    spr.fillRoundRect(0, 2, 128, 25, 5, 0x529d);
    spr.fillRoundRect(0, 28, 128, 25, 5, 0x751f);
    spr.fillRoundRect(0, 54, 128, 25, 5, 0x36ae);
    spr.fillRoundRect(0, 80, 128, 25, 5, 0xfb50);
    spr.fillRoundRect(0, 106, 128, 25, 5, 0xfbea);
    spr.fillRoundRect(0, 132, 128, 25, 5, 0xfd20);

    spr.setTextColor(TFT_WHITE);
    spr.drawString(wk[(currentWeek + 1) % 7], 5, 3);
    spr.drawString(wk[(currentWeek + 2) % 7], 5, 29);
    spr.drawString(wk[(currentWeek + 3) % 7], 5, 55);
    spr.drawString(wk[(currentWeek + 4) % 7], 5, 81);
    spr.drawString(wk[(currentWeek + 5) % 7], 5, 107);
    spr.drawString(wk[(currentWeek + 6) % 7], 5, 133);

    spr.drawString(futureWeather.day1wea, 63, 3);
    spr.drawString(futureWeather.day2wea, 63, 29);
    spr.drawString(futureWeather.day3wea, 63, 55);
    spr.drawString(futureWeather.day4wea, 63, 81);
    spr.drawString(futureWeather.day5wea, 63, 107);
    spr.drawString(futureWeather.day6wea, 63, 133);

    spr.pushSprite(0, 0);
    spr.deleteSprite();
    spr.unloadFont();
}

/**
 * @name BILIBILI_UI
 * @brief BILIBILI_UI
 * @author levi5
*/
void BILIBILI_UI(void)
{
    if(pushFlag)
    {
        Serial.println("BILIBILI");
        Serial.println(pushFlag);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.pushImage(0, 0, 128, 60, icon_bili);
        pushFlag = false;
    }
    spr.createSprite(128, 95);
    spr.loadFont(BoxedRegular18);
    spr.fillRoundRect(3, 6, 5, 36, 2, 0x063d);
    spr.fillRoundRect(3, 53, 5, 36, 2, 0x063d);
    spr.setTextDatum(TL_DATUM);
    spr.setTextColor(TFT_WHITE);
    spr.drawString("Following: ", 11, 6);
    spr.drawString(num2str(followingCount), 11, 27);
    spr.drawString("Follower: ", 11, 53);
    spr.drawString(num2str(fansCount), 11, 74);

    spr.pushSprite(0, 60);
    spr.deleteSprite();
    spr.unloadFont();
}

/**
 * @name BRIGHTNESS_UI
 * @brief BRIGHTNESS_UI
*/
void BRIGHTNESS_UI(void)
{
    if(pushFlag)
    {
        Serial.println("BRIGHTNESS");
        Serial.println(pushFlag);
        tft.setTextColor(TFT_CYAN);
        pushFlag = false;
    }
    bar = (brightness - 10) / 24;
    set_back_brightness(brightness);
    spr.createSprite(128, 160);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TC_DATUM);
    spr.loadFont(BoxedRegular18);
    spr.drawString("BRIGHTNESS", 64, 5);
    spr.unloadFont();
    spr.drawRect(10, 30, 108, 40, TFT_WHITE);
    spr.drawRect(10, 75, 108, 82, TFT_WHITE);
    spr.loadFont(DigitalNumbers30);
    spr.drawString(num2str(brightness), 64, 35);
    spr.unloadFont();
    for(int i = 0; i < 10; i++)
    {
        if(i < bar)
        {
            spr.fillRect(15, 145 - i * 7, 98, 5, 0xfa00);
        }
        else
        {
            spr.fillRect(15, 145 - i * 7, 98, 5, 0xa140);
        }
    }// 82 89 96 103 110 117 124 131 138 145
    // 10 34 58 82 106 130 154 178 202 226 250
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}
void WIFI_UI(void)
{
    if(pushFlag)
    {
        Serial.println("WIFI");
        Serial.println(pushFlag);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.pushImage(23, 39, 80, 80, icon_wifi);
        pushFlag = false;
    }
}
void BLUETOOTH_UI(void)
{
    if(pushFlag)
    {
        Serial.println("BLUETOOTH");
        Serial.println(pushFlag);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_CYAN);
        tft.pushImage(23, 39, 80, 80, icon_bluetooth);
        pushFlag = false;
    }
}
void ABOUT_UI(void)
{
    if(pushFlag)
    {
        Serial.println("ABOUT");
        Serial.println(pushFlag);
        tft.setTextColor(TFT_CYAN);
        pushFlag = false;
    }
    spr.createSprite(128, 150);
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM);
    spr.loadFont(BoxedRegular18);//10
    spr.pushImage(31, 2, 64, 64, icon_head);
    spr.drawString("ESP32 UV BOX", 0, 70);
    spr.drawString("author: levi5", 0, 90);
    spr.drawString("version: .v1", 0, 110);
    spr.unloadFont();
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}
/**
 * @name set_weather_icon
 * @brief 设置天气图标
 * @param icon 天气图标
 * @author levi5
*/
void set_weather_icon(int icon)
{
    switch(icon)
    {
        case 100:
        case 150:
            spr.pushImage(0, 20, 50, 50, icon_sunny);
            break;
        case 101:
        case 102:
        case 103:
        case 151:
        case 152:
        case 153:
            spr.pushImage(0, 20, 50, 50, icon_cloudy);
            break;
        case 104:
            spr.pushImage(0, 20, 50, 50, icon_overcast);
            break;
        case 305:
        case 309:
        case 313:
        case 314:
        case 399:
            spr.pushImage(0, 20, 50, 50, icon_light_rain);
            break;
        case 300:
        case 301:
            spr.pushImage(0, 20, 50, 50, icon_shower_rain);
            break;
        case 302:
        case 303:
        case 304:
            spr.pushImage(0, 20, 50, 50, icon_thundershower);
            break;
        case 306:
        case 307:
        case 308:
        case 310:
        case 311:
        case 312:
        case 315:
        case 316:
        case 317:
        case 318:
        case 350:
        case 351:
            spr.pushImage(0, 20, 50, 50, icon_heavy_rain);
            break;
        case 400:
        case 401:
        case 402:
        case 403:
        case 404:
        case 405:
        case 406:
        case 407:
        case 408:
        case 409:
        case 410:
        case 456:
        case 457:
        case 499:
            spr.pushImage(0, 20, 50, 50, icon_light_snow);
            break;
        case 500:
        case 501:
        case 509:
        case 510:
        case 514:
        case 515:
            spr.pushImage(0, 20, 50, 50, icon_foggy);
            break;
        case 502:
        case 511:
        case 512:
        case 513:
            spr.pushImage(0, 20, 50, 50, icon_haze);
            break;
        case 503:
        case 504:
            spr.pushImage(0, 20, 50, 50, icon_sand);
            break;
        case 507:
        case 508:
            spr.pushImage(0, 20, 50, 50, icon_sandstorm);
            break;
        case 900:
            spr.pushImage(0, 20, 50, 50, icon_sunny);// hot
            break;
        case 901:
            spr.pushImage(0, 20, 50, 50, icon_light_snow);// cold
            break;
        case 999:
        default:
            spr.pushImage(0, 20, 50, 50, icon_null);
            break;
    }
}