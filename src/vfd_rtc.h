#ifndef vfd_rtc_h
#define vfd_rtc_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <M5Unified.h>

#include <time.h>
#include <sys/time.h>
#include <sntp.h>

//#include "time.h"
#include "RTClib.h"

#define NTPSERVER "ntp.nict.jp"   // NTPサーバ
#define RTC_PLS_GPIO  40          // RTC GPIO割り込み入力端子

/*
class Rtccont{
  public:
    Rtccont(void);        // コンストラクタ
    void timesync(struct tm time);  // RTC時刻書込み
    void timedisp(void);
    void gettime(RTC_TimeTypeDef *,RTC_DateTypeDef *);
//    bool timeread(tmElements_t &time);
  
  private:
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
//    portMUX_TYPE mutex_rtc = portMUX_INITIALIZER_UNLOCKED; // Mutex
//    SemaphoreHandle_t xSemaphore;
    
};*/
class RtcCont{
  public:
    RtcCont(void);        // コンストラクタ
    ~RtcCont(void);
//    void init(void);
    void timeSync(struct tm timeInfo);  // RTC時刻書込み
//    void timeDisp(struct tm timeInfo);
    uint8_t timeRead(struct tm *timeInfo);
  private:
};
GLOBAL bool ntpSetup;           // RTC設定要求

class LocalTimeCont{
  public:
    void init(void);
    void timeZoneSet(void);
    void timeSync(struct tm timeInfo);  // RTC時刻設定
    uint8_t timeRead(struct tm *timeInfo);
    void timeDisp(void);
  private:

};
GLOBAL LocalTimeCont localTimeCont;

class SntpCont{
  public:
    void init(void);
  private:

};
GLOBAL SntpCont sntpCont;
GLOBAL bool sntpCompleted;    // SNTP同期完了フラグ
void setNtpTime(void);        // NTP時刻取得

// システム時刻管理
class SystemTimeCont{
  public:
    SystemTimeCont(void);
    ~SystemTimeCont(void);
    void init(struct tm *timeInfo);   // 初期化
    time_t read(void);                // 読み出し
    void timeAdd(void);               // +1
    void write(struct tm timeInfo);   // システム時刻設定
  private:
    portMUX_TYPE systimeMutex = portMUX_INITIALIZER_UNLOCKED; // Mutex
    time_t sysTime;                   // システム用時刻情報
};
//GLOBAL SystemTimeCont sysTimCnt;

GLOBAL volatile SemaphoreHandle_t rtcPalsSemaphore;   // RTCパルス計時要求

#undef GLOBAL
#endif