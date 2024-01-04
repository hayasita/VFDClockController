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
  int writtenChars;

  if(device == DEVICE_ENVIII_TEMP){
    writtenChars = snprintf(tmp, sizeof(tmp),"TEMP:%.2f",devData.env3Temperature);
  }
  else if(device == DEVICE_ENVIII_HUMI){
    writtenChars = snprintf(tmp, sizeof(tmp),"HUMI:%.2f",devData.env3Humidity);
  }
  else if(device == DEVICE_ENVIII_PRESS){
    writtenChars = snprintf(tmp, sizeof(tmp),"PRES:%.2f",(devData.env3Pressure / 100));
  }
  else if(device == DEVICE_ILLUMI){
    writtenChars = snprintf(tmp, sizeof(tmp),"ILLU:%d",devData.illumiData);
  }
  else if(device == DEVICE_DCDC){
    writtenChars = snprintf(tmp, sizeof(tmp),"DCTR:%d DCFB:%d",devData.dcdcTrg,devData.dcdcFdb);
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
