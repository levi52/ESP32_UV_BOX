/**
 * @file sensor.cpp
 * @brief 传感器 ADC
 * @author levi5
 * @version 1.0
 * @date 2024-05-20
 * @copyright Copyright (c) 2024, levi5
 * @details
 * @par 修改日志
*/

#include "sensor.h"

int UVPIN = 33;
int BATTERYPIN = 34;

BMP280 bmp280;
AHT20 aht20;
AHT20Data lastValidData;
/**
 * @name sensor_aht20_init
 * @brief 初始化aht20
 * @author levi5
*/
void sensor_aht20_init(void)
{
    if (aht20.begin() == false)
    {
        Serial.println("AHT20 not detected. Please check wiring.");
    }
    Serial.println("AHT20 acknowledged.");
}

/**
 * @name sensor_aht20_data
 * @brief 获取aht20数据：温度，湿度
 * @author levi5
 * @return AHT20Data结构体，包含温度和湿度。
*/
AHT20Data sensor_aht20_data(void)
{
    AHT20Data aData;
    if (aht20.available() == true)
    {
        // 获取温湿度
        aData.humidity = aht20.getHumidity();
        delay(100);
        
        aData.temperature = aht20.getTemperature();
        
        // 检查温度是否超出合理范围
        if (aData.temperature > 1000)
        {
            // 温度超出范围，返回上一次有效数据
            aData = lastValidData;
        }
        else
        {
            // 温度在合理范围内，更新上一次有效数据并返回
            lastValidData = aData;
        }
    }
    else
    {
        // 如果传感器不可用，返回上一次有效数据
        aData = lastValidData;
    }

    return aData;
}

/**
 * @name sensor_bmp280_init
 * @brief 初始化bmp280
 * @author levi5
*/
void sensor_bmp280_init(void)
{
    if(!bmp280.init()){
        Serial.println("BMP280 not detected. Please check wiring.");
    }
    Serial.println("BMP280 acknowledged.");
}

/**
 * @name sensor_bmp280_data
 * @brief 获取bmp280数据：温度，压强，海拔
 * @author levi5
 * @return BMP280Data结构体，包含温度、压力和海拔数据。
*/
BMP280Data sensor_bmp280_data(void)
{
    BMP280Data bData;
    bData.temperature = bmp280.getTemperature();
    delay(100);
    bData.pressure = bmp280.getPressure();
    delay(100);
    bData.altitude = bmp280.calcAltitude(bData.pressure);
    return bData;
}

/**
 * @name sensor_uv_init
 * @brief 初始化UV ADC引脚
 * @author levi5
*/
void sensor_uv_init(void)
{
    Serial.println("uv sensor init");
    pinMode(UVPIN, INPUT);
}

/**
 * @name sensor_uv_data
 * @brief 读取UV ADC电压值
 * @author levi5
 * @return uint8_t 返回紫外线强度级别，范围为0到11。
*/
uint8_t sensor_uv_data(void)
{
    //读取GPIO33的模拟值
    uint16_t uvValue = 0;
    float uvVoltage = 0;
    uint8_t uvLevel;
    uvValue = analogRead(UVPIN);
    uvVoltage = uvValue/4095.0*3300.0*2;
    //通过串口0 打印出读取到的值
    // Serial.println(uvValue);
    // Serial.println(uvVoltage);
    if(uvVoltage>= 65 && uvVoltage < 297)
    {
        uvLevel = 1;
    }
    else if(uvVoltage>= 297 && uvVoltage < 419)
    {
        uvLevel = 2;
    }
    else if(uvVoltage>= 419 && uvVoltage < 535)
    {
        uvLevel = 3;
    }
    else if(uvVoltage>= 535 && uvVoltage < 664)
    {
        uvLevel = 4;
    }
    else if(uvVoltage>= 664 && uvVoltage < 800)
    {
        uvLevel = 5;
    }
    else if(uvVoltage>= 800 && uvVoltage < 915)
    {
        uvLevel = 6;
    }
    else if(uvVoltage>= 915 && uvVoltage < 1044)
    {
        uvLevel = 7;
    }
    else if(uvVoltage>= 1044 && uvVoltage < 1160)
    {
        uvLevel = 8;
    }
    else if(uvVoltage>= 1160 && uvVoltage < 1290)
    {
        uvLevel = 9;
    }
    else if(uvVoltage>= 1290 && uvVoltage < 1424)
    {
        uvLevel = 10;
    }
    else if(uvVoltage>= 1424 && uvVoltage < 1547)
    {
        uvLevel = 11;
    }
    else if(uvVoltage>= 1547)
    {
        uvLevel = 12;
    }
    else
    {
        uvLevel = 0;
    }
    return uvLevel;
}

/**
 * @name sensor_battery_init
 * @brief 初始化BATTERY ADC引脚
 * @author levi5
*/
void sensor_battery_init(void)
{
    Serial.println("battery init");
    pinMode(BATTERYPIN, INPUT);
}

/**
 * @name sensor_battery_data
 * @brief BATTERY ADC电压值
 * @author levi5
 * @return float 返回电池电压值
*/
float sensor_battery_data(void)
{
    //读取GPIO34的模拟值
    uint16_t batteryValue = 0;
    float batteryVoltage = 0;
    batteryValue = analogRead(BATTERYPIN);
    batteryVoltage = batteryValue/4095.0*3.3*4;
    //通过串口0 打印出读取到的值
    // Serial.println(batteryValue);
    // Serial.println(batteryVoltage);
    return batteryVoltage;
}