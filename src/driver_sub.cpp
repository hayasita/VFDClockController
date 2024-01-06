/**
 * @file driver_sub.cpp
 * @author hayasita04@gmail.com
 * @brief driverで使用するデータを作成する関数群
 * @version 0.1
 * @date 2024-01-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
//#include <M5Unified.h>
#include "driver_sub.h"

/**
 * @brief 日時表示データを作成する。
 * 
 * @param buffer 作製した表示データを入れる
 * @param timeinfo 時刻データ
 * @param title 先頭に付けるタイトル文字列
 * @return true 成功
 * @return false 失敗
 */
bool dispDateTime(char* buffer,tm timeinfo,const char* title)
{
  bool ret = true;
  char tmp[50];

  int writtenChars = snprintf(tmp, sizeof(tmp)
    ,"%s%lu/%02d/%02d %02d:%02d:%02d"
    ,title
    ,timeinfo.tm_year+1900,timeinfo.tm_mon+1,timeinfo.tm_mday
    ,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
  if((writtenChars >= 0 && writtenChars < sizeof(tmp))){
    ret = true;
    strcpy(buffer,tmp);
  }
  else{
    ret = false;
    strcpy(buffer,"Error!!");
  }

  return ret;
}

/**
 * @brief デバイス情報の表示データを作成する。
 * 
 * @param buffer 作製した表示データを入れる
 * @param devData デバイスデータ
 * @param device 出力センサ指定
 * @return true データ作成成功
 * @return false データ作成失敗
 */
bool dispDeviceData(char* buffer,DeviceData devData,uint8_t device)
{
  bool ret = true;
  char tmp[50];
  char title[][5] = {"TEMP","HUMI","PRES","ILLU","Gas ","Alti","DCTR","DCFB"};
  char sensorDat1[8],sensorDat2[8];
  char noData6[6] = "--.--";
  char noData8[8] = "----.--";
  int writtenChars;

  if(device == DEVICE_ENVIII_TEMP){
    if(devData.enviiiData.sensorActive == true){
      writtenChars = snprintf(sensorDat1, sizeof(sensorDat1),"%2.2f",devData.enviiiData.env3Temperature);
    }
    else{
      writtenChars = snprintf(sensorDat1, sizeof(sensorDat1),"%s",noData6);
    }
    writtenChars = snprintf(sensorDat2, sizeof(sensorDat2),"%2.2f",devData.bme680Data.temperature);
    writtenChars = snprintf(tmp, sizeof(tmp),"%s:%s:%s *c",title[device],sensorDat1,sensorDat2);
  }
  else if(device == DEVICE_ENVIII_HUMI){
    if(devData.enviiiData.sensorActive == true){
      writtenChars = snprintf(sensorDat1, sizeof(sensorDat1),"%2.2f",devData.enviiiData.env3Humidity);
    }
    else{
      writtenChars = snprintf(sensorDat1, sizeof(sensorDat1),"%s",noData6);
    }
    writtenChars = snprintf(sensorDat2, sizeof(sensorDat2),"%2.2f",devData.bme680Data.humidity);
    writtenChars = snprintf(tmp, sizeof(tmp),"%s:%s:%s",title[device],sensorDat1,sensorDat2);
  }
  else if(device == DEVICE_ENVIII_PRESS){
    if(devData.enviiiData.sensorActive == true){
      writtenChars = snprintf(sensorDat1, sizeof(sensorDat1),"%4.2f",(devData.enviiiData.env3Pressure));
    }
    else{
      writtenChars = snprintf(sensorDat1, sizeof(sensorDat1),"%s",noData8);
    }
    writtenChars = snprintf(sensorDat2, sizeof(sensorDat2),"%4.2f",(devData.bme680Data.pressure/100.0));
    writtenChars = snprintf(tmp, sizeof(tmp),"%s:%s:%s",title[device],sensorDat1,sensorDat2);
  }
  else if(device == DEVICE_GAS){
    if(devData.bme680Data.pressure > 0){
      writtenChars = snprintf(tmp, sizeof(tmp),"%s:%5.2f KOhms",title[device],devData.bme680Data.gasResistance/1000.0);
    }
    else{
      writtenChars = snprintf(tmp, sizeof(tmp),"%s:000.00 KOhms",title[device]);
    }
  }
  else if(device == DEVICE_ALTITUBE){
    writtenChars = snprintf(tmp, sizeof(tmp),"%s:%.2fm",title[device],devData.bme680Data.altitude);
  }
  else if(device == DEVICE_ILLUMI){
    writtenChars = snprintf(tmp, sizeof(tmp),"ILLU:%4d",devData.illumiData);
  }
  else if(device == DEVICE_DCDC){
    writtenChars = snprintf(tmp, sizeof(tmp),"DCTR:%4d DCFB:%4d",devData.dcdcTrg,devData.dcdcFdb);
  }
  else{
    writtenChars = -1;
    strcpy(buffer,"Device Number Error!!");
  }

  if((writtenChars >= 0 && writtenChars < sizeof(tmp))){
    ret = true;
    strcpy(buffer,tmp);
  }
  else{
    ret = false;
    strcpy(buffer,"Error!!");
  }

  return ret;
}
