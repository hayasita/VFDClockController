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
