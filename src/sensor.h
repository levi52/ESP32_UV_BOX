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

#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <Arduino.h>
#include <AHT20.h>
#include "Seeed_BMP280.h"

struct AHT20Data
{
    float temperature;
    float humidity;
};
struct BMP280Data
{
    float temperature;
    float pressure;
    float altitude;
};

void sensor_aht20_init(void);
AHT20Data sensor_aht20_data(void);
void sensor_bmp280_init(void);
BMP280Data sensor_bmp280_data(void);
void sensor_uv_init(void);
uint8_t sensor_uv_data(void);
void sensor_battery_init(void);
float sensor_battery_data(void);
#endif