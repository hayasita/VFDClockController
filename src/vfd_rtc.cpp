#include "vfd_rtc.h"
#include "vfd_conf.h"
#include "vfd_driver.h"
#include "vfd_wificnt.h"

void timePrint(struct tm timeInfo)
{
  char txt[25];//文字格納用
  sprintf(txt, "RTC: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)timeInfo.tm_year), timeInfo.tm_mon, timeInfo.tm_mday,
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);//人間が読める形式に変換
  Serial.println(txt);

  return;
}

// -----

/**
 * @brief RTCのパルス割り込みのカウント用変数
 * 
 */
volatile static uint16_t hzCount;
/**
 * @brief RTCパルスによる計時　割り込み処理
 * 
 */
void IRAM_ATTR rtcPlusCount(void){
  hzCount++;
//  if(hzCount >= 32768){
    hzCount = 0;
//    Serial.println("rtcPlusCount!!");

    xSemaphoreGiveFromISR(rtcPalsSemaphore, NULL);    // RTCパルス計時要求
//  }
}

#ifdef RTCUNIT_DS1307
/**
 * @brief Construct a new Rtc Cont:: Rtc Cont object
 * 
 */
RtcCont::RtcCont(void)
{
  // RTC有無判別
  if(deviceChk.rtc() != true){
    Serial.println("Couldn't find RTC. I2C Device");
    return;
  }

  Wire.setPins(SDA_PIN,SCL_PIN);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC. Wire Begin");
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(2022, 1, 1, 8, 0, 0));  // 仮設定
  }
  else{
    Serial.println("RTC is running!");
  }

  // RTCパルス出力設定
//  rtc.writeSqwPinMode(DS1307_SquareWave32kHz);  // 32kHz square wave
  rtc.writeSqwPinMode(DS1307_SquareWave1HZ);  // 1Hz square wave

  // パルスカウント処理初期化
  hzCount = 0;                                            // パルスカウンタ初期化
  pinMode(RTC_PLS_GPIO, INPUT);                           // パルス入力ポート初期化
  attachInterrupt(RTC_PLS_GPIO, rtcPlusCount, FALLING);   // パルス入力割り込み設定

  ntpSetup = false;     // RTC設定要求初期化

  return;
}

/**
 * @brief Destroy the Rtc Cont:: Rtc Cont object
 * 
 */
RtcCont::~RtcCont(void)
{
  Serial.println("-- RtcCont::デストラクタ --");

  return;
}

/**
 * @brief RTC 時刻読み出し　返値をboolに変更すべき
 * 
 * @param timeInfo 時刻読み出し結果
 * @return uint8_t 読み出し成功・失敗。
 */
bool RtcCont::timeRead(struct tm *timeInfo)
{
  uint8_t ret = true;

  if(deviceChk.rtc() != true){
//    Serial.println("Couldn't find RTC");
    time_t now;
    time(&now);
    localtime_r(&now, timeInfo);

    return false;
  }

  DateTime now = rtc.now();
  if(now.year() == 2000U){
    ret = false;
  }
  // RTC読出し情報設定
  timeInfo->tm_year = now.year() - 1900;
  timeInfo->tm_mon = now.month() - 1;
  timeInfo->tm_mday = now.day();
  timeInfo->tm_hour = now.hour();
  timeInfo->tm_min = now.minute();
  timeInfo->tm_sec = now.second();

  return ret;
}

/**
 * @brief RTCに時刻を設定する
 * 
 * @param timeInfo 設定時刻
 */
void RtcCont::timeSync(struct tm timeInfo)
{

  Serial.println("-- RtcCont::timeSync --");
  if(deviceChk.rtc() != true){
    Serial.println("Couldn't find RTC");
    return;
  }

  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");

  rtc.adjust(DateTime(timeInfo.tm_year % 100, timeInfo.tm_mon+1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec));

  Serial.println("RtcCont::timeSync : Conplete");
  return;
}
#elif defined RTCUNIT_DS3231
/**
 * @brief Construct a new Rtc Cont:: Rtc Cont object
 * 
 */
RtcCont::RtcCont(void)
{
  // RTC有無判別
  if(deviceChk.rtc() != true){
    Serial.println("Couldn't find RTC. I2C Device");
    return;
  }

  Wire.setPins(SDA_PIN,SCL_PIN);

  if (rtc.lostPower()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(2022, 1, 1, 8, 0, 0));  // 仮設定
  }
  else{
    Serial.println("RTC is running!");
  }

  // RTCパルス出力設定
//  rtc.writeSqwPinMode(DS1307_SquareWave32kHz);  // 32kHz square wave
//  rtc.writeSqwPinMode(DS1307_SquareWave1HZ);  // 1Hz square wave
  rtc.enable32K();    // Enable 32KHz Output

  // パルスカウント処理初期化
  hzCount = 0;                                            // パルスカウンタ初期化
  pinMode(RTC_PLS_GPIO, INPUT);                           // パルス入力ポート初期化
  attachInterrupt(RTC_PLS_GPIO, rtcPlusCount, FALLING);   // パルス入力割り込み設定

  ntpSetup = false;     // RTC設定要求初期化

  return;
}

/**
 * @brief Destroy the Rtc Cont:: Rtc Cont object
 * 
 */
RtcCont::~RtcCont(void)
{
  Serial.println("-- RtcCont::デストラクタ --");

  return;
}

/**
 * @brief RTC 時刻読み出し　返値をboolに変更すべき
 * 
 * @param timeInfo 時刻読み出し結果
 * @return uint8_t 読み出し成功・失敗。
 */
bool RtcCont::timeRead(struct tm *timeInfo)
{
  uint8_t ret = true;

  if(deviceChk.rtc() != true){
//    Serial.println("Couldn't find RTC");
    time_t now;
    time(&now);
    localtime_r(&now, timeInfo);

    return false;
  }

  DateTime now = rtc.now();
  if(now.year() == 2000U){
    ret = false;
  }
  // RTC読出し情報設定
  timeInfo->tm_year = now.year() - 1900;
  timeInfo->tm_mon = now.month() - 1;
  timeInfo->tm_mday = now.day();
  timeInfo->tm_hour = now.hour();
  timeInfo->tm_min = now.minute();
  timeInfo->tm_sec = now.second();

  return ret;
}

/**
 * @brief RTCに時刻を設定する
 * 
 * @param timeInfo 設定時刻
 */
void RtcCont::timeSync(struct tm timeInfo)
{

  Serial.println("-- RtcCont::timeSync --");
  if(deviceChk.rtc() != true){
    Serial.println("Couldn't find RTC");
    return;
  }

  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");

  rtc.adjust(DateTime(timeInfo.tm_year % 100, timeInfo.tm_mon+1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec));

  Serial.println("RtcCont::timeSync : Conplete");
  return;
}
#endif
// -----

void LocalTimeCont::init(void)
{
  time_t now;
  char strftime_buf[64];
  struct tm timeInfo;

  Serial.println("-- localTimeInit --");

  timeZoneSet();
  RtcCont *rtc = new RtcCont;
  rtc->timeRead(&timeInfo);
  Serial.println(timeInfo.tm_year);
  Serial.println("-- RtcTimeDisp --");
//  rtc->timeDisp(timeInfo);
  timePrint(timeInfo);
  delete rtc;

  timeSync(timeInfo);

  return;
}

void LocalTimeCont::timeZoneSet(void)
{
  time_t now;
  char strftime_buf[64];
  struct tm timeInfo;
  Serial.println("-- LocalTimeCont::timeZoneSet --");
  
  setenv("TZ", confDat.getTimeZoneData(), 1);
  tzset();

  time(&now);
  localtime_r(&now, &timeInfo);
//  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeInfo);
//  Serial.println(strftime_buf);
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");

  return;
}

void LocalTimeCont::timeSync(struct tm timeInfo)
{
  time_t now;
  char strftime_buf[64];
  time_t timertc = mktime(&timeInfo);
  Serial.println("-- LocalTimeCont::timeSync --");

  struct timeval tv ={
    .tv_sec = timertc
  };
  settimeofday(&tv,NULL);   // システムの時刻を指定した時間に設定する

  time(&now);
  localtime_r(&now, &timeInfo);
//  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeInfo);
//  Serial.println(strftime_buf);
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");

  return;
}

void LocalTimeCont::timeDisp(void)
{
  time_t now;
  struct tm timeInfo;
  char strftime_buf[64];

  time(&now);
  localtime_r(&now, &timeInfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeInfo);
  Serial.println("-- timeDisp --");
  Serial.println(strftime_buf);

  return;
}

// Callback: set NTP sync-time into RTC
void SntpTimeSyncNotificationCallback(struct timeval *tv)
{
  sntp_sync_status_t sntp_sync_status = sntp_get_sync_status();
  Serial.println("== SntpTimeSyncNotificationCallback ==");
  Serial.print("sntp_sync_status:");
  Serial.println(sntp_sync_status);

  if (sntp_sync_status == SNTP_SYNC_STATUS_COMPLETED)
  {
    Serial.println("-- SNTP_SYNC_STATUS_COMPLETED --");
    
    ntpSetup = true;                // NTP時刻 → RTC設定要求
    sntpCompleted = true;           // SNTP同期完了フラグ設定
    eventSntpSync = true;           // SNTP同期完了イベント要求
  }
  else{
    Serial.println("callback else");
  }
  return;
}

void SntpCont::init(void)
{
//  const char* time_zone  = confDat.getTimeZoneData();
//  const char* ntp_server = "pool.ntp.org";

  Serial.println("-- SntpCont::init --");
  Serial.println(confDat.getTimeZoneData());
  if(confDat.getNtpset() == 1){
    sntp_set_sync_mode ( SNTP_SYNC_MODE_IMMED );    // 同期モード設定　すぐに更新
    sntp_set_sync_interval(1*3600*1000); // 1 hours
//    sntp_set_sync_interval(2*60*1000); // 60Sec

//    configTzTime(time_zone, ntp_server);
    setNtpTime();

//  status
    Serial.printf("setup: SNTP sync mode = %d (0:IMMED 1:SMOOTH)\n", sntp_get_sync_mode());
    Serial.printf("setup: SNTP sync status = %d (1:SNTP_SYNC_STATUS_RESET 2:SNTP_SYNC_STATUS_IN_PROGRESS 3:SNTP_SYNC_STATUS_COMPLETED)\n", sntp_get_sync_status());
    Serial.printf("setup: SNTP sync interval = %dms\n", sntp_get_sync_interval());

    // set sntp callback
    sntp_set_time_sync_notification_cb(SntpTimeSyncNotificationCallback);
  }

  sntpCompleted = false;    // SNTP同期完了フラグ初期化

  return;
}

// NTP時刻取得
//    NTPから時刻を取得し、RTCに設定する。
void setNtpTime(void)
{
//  const char* ntp_server = "pool.ntp.org";
//  Serial.println("-- setNtpTime --");

  // タイムゾーン設定
  setenv("TZ", confDat.getTimeZoneData(), 1);
  tzset();

//  configTzTime(confDat.getTimeZoneData(), "time.google.com");
  configTzTime(confDat.getTimeZoneData(), NTPSERVER);
  eventAutoConSntpReq = true;

  return;
}

// -----

SystemTimeCont::SystemTimeCont(void)
{
  Serial.println("-- SystemTimeCont::コンストラクタ --");

  return;
}

SystemTimeCont::~SystemTimeCont(void)
{
  Serial.println("-- SystemTimeCont::デストラクタ --");

  return;
}

// システム時刻情報起動時処理
void SystemTimeCont::init(struct tm *timeInfo)
{
//  time_t sysTimeTmp;

  systimeMutex = portMUX_INITIALIZER_UNLOCKED;    // mutex初期化

  // タイムゾーン設定
  setenv("TZ", confDat.getTimeZoneData(), 1);
  tzset();

  // システム時刻情報設定
  write(*timeInfo);

  // ROS時刻情報設定
  struct timeval tv ={
    .tv_sec = read()
  };
  settimeofday(&tv,NULL);   // ROSの時刻を指定した時間に設定する

  // Debug
  char txt[55];//文字格納用
  sprintf(txt, "SystemTimeCont::init: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)timeInfo->tm_year), timeInfo->tm_mon, timeInfo->tm_mday,
          timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);//人間が読める形式に変換
  Serial.println(txt);
//  Serial.println(sysTimeTmp);

  return;
}

// システム時刻情報読み出し
time_t SystemTimeCont::read(void)
{
  time_t sysTimeTmp;

  portENTER_CRITICAL(&systimeMutex);
  sysTimeTmp = sysTime;
  portEXIT_CRITICAL(&systimeMutex);

  struct tm *timeInfo = localtime(&sysTimeTmp);
/*
  char txt[50];//文字格納用
  sprintf(txt, "sysTimeRead: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)timeInfo->tm_year), timeInfo->tm_mon, timeInfo->tm_mday,
          timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);//人間が読める形式に変換
  Serial.println(txt);
*/
  return sysTimeTmp;
}

// システム時刻情報設定
void SystemTimeCont::write(struct tm timeInfo)
{
  time_t sysTimeTmp;

  // システム用時刻情報設定
  sysTimeTmp = mktime(&timeInfo);
  portENTER_CRITICAL(&systimeMutex);
  sysTime = sysTimeTmp;
  portEXIT_CRITICAL(&systimeMutex);

  // Debug
  struct tm *timeInfo2 = localtime(&sysTimeTmp);
  char txt[50];//文字格納用
  sprintf(txt, "sysTimewrite: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)timeInfo2->tm_year), timeInfo2->tm_mon, timeInfo2->tm_mday,
          timeInfo2->tm_hour, timeInfo2->tm_min, timeInfo2->tm_sec);//人間が読める形式に変換
  Serial.println(txt);

  return;
}

/**
 * @brief システム時刻情報 +1
 * 
 */
void SystemTimeCont::timeAdd(void)
{
  portENTER_CRITICAL(&systimeMutex);
  sysTime++;
  portEXIT_CRITICAL(&systimeMutex);

//  Serial.println(sysTime);
  return;
}


