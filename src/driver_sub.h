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
#include <cstdio>
#include <ctime>
#include <cstring>

bool dispDateTime(char* buffer,tm timeinfo,const char* title);

class DeviceData{
  public:
    float env3Temperature;      // 気温
    float env3Humidity;         // 湿度
    float env3Pressure;         // 気圧
    uint16_t illumiData;        // 周辺輝度
    uint16_t dcdcTrg;           // DCDC目標値
    uint16_t dcdcFdb;           // DCDCフィードバック値
};
#define DEVICE_ENVIII_TEMP    0
#define DEVICE_ENVIII_HUMI    1
#define DEVICE_ENVIII_PRESS   2
#define DEVICE_ILLUMI         3
#define DEVICE_DCDC           4

bool dispDeviceData(char* buffer,DeviceData devData,uint8_t sensor);
