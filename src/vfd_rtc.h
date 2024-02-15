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

// RTC Unit Select
//#define RTCUNIT_DS1307
#define RTCUNIT_DS3231

class RtcCont{
  public:
    RtcCont(void);
    ~RtcCont(void);
    void timeSync(struct tm timeInfo);  // RTC時刻書込み
    bool timeRead(struct tm *timeInfo); // RTC時刻読出し
  private:
#ifdef RTCUNIT_DS1307
    RTC_DS1307 rtc;
#elif defined RTCUNIT_DS3231
    RTC_DS3231 rtc;
#endif
};
GLOBAL bool ntpSetup;           // RTC設定要求

GLOBAL volatile SemaphoreHandle_t rtcPalsSemaphore;   // RTCパルス計時要求

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

#undef GLOBAL
#endif