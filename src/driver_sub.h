/**
 * @file driver_sub.h
 * @author hayasita04@gmail.com
 * @brief driverで使用するデータを作成する関数群のヘッダ
 * @version 0.1
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef driver_sub_h
#define driver_sub_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <cstdio>
#include <ctime>
#include <cstring>

bool dispDateTime(char* buffer,tm timeinfo,const char* title);

class SensorENVIIIData{
  public:
    bool sensorActive;
    float env3Temperature;      // ENVIII SENSOR 気温
    float env3Humidity;         // ENVIII SENSOR 湿度
    float env3Pressure;         // ENVIII SENSOR 気圧
};

class SensorBME680Data{
  public:
    bool sensorActive;
    float temperature;      // BME680 SENSOR 気温
    float humidity;         // BME680 SENSOR 湿度
    float pressure;         // BME680 SENSOR 気圧
    uint32_t gasResistance; // BME680 SENSOR ガス
    float altitude;         // 
};

class DeviceData{
  public:
    SensorENVIIIData enviiiData;  // ENVIII SENSOR情報
    SensorBME680Data bme680Data;  // BME680 SENSOR情報
    uint16_t illumiData;          // 周辺輝度
    uint16_t dcdcTrg;             // DCDC目標値
    uint16_t dcdcFdb;             // DCDCフィードバック値
};
#define DEVICE_ENVIII_TEMP    0
#define DEVICE_ENVIII_HUMI    1
#define DEVICE_ENVIII_PRESS   2
#define DEVICE_ILLUMI         3
#define DEVICE_GAS            4
#define DEVICE_ALTITUBE       5
#define DEVICE_DCDC           6
#define DEVICE_DCDC2          7   // DEVICE_DCDCで使用するので欠番


bool dispDeviceData(char* buffer,DeviceData devData,uint8_t sensor);

#undef GLOBAL
#endif