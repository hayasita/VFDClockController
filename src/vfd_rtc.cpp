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


#ifdef ARDUINO_M5Stick_C

/*
Rtccont::Rtccont(void)
{
  RTC_TimeTypeDef TimeStruct;
  TimeStruct.Hours   = 18;  //Set the time.  设置时间
  TimeStruct.Minutes = 56;
  TimeStruct.Seconds = 10;
  M5.Rtc.SetTime(&TimeStruct);  //writes the set time to the real time clock.  将设置的时间写入实时时钟
  RTC_DateTypeDef DateStruct;
  DateStruct.WeekDay = 3; //Set the date.  设置日期
  DateStruct.Month = 3;
  DateStruct.Date = 22;
  DateStruct.Year = 2019;
  M5.Rtc.SetData(&DateStruct);  //writes the set date to the real time clock.  将设置的日期写入实时时钟
  
//  xSemaphore = xSemaphoreCreateBinary();
  return;
}
*/
RtcCont::RtcCont(void)
{

  init();

  return;
}

RtcCont::~RtcCont(void)
{
  Serial.println("-- RtcCont::デストラクタ --");

  return;
}

void RtcCont::init(void)
{
  Serial.println("-- RtcCont::init --");
/*  
  RTC_DateTypeDef DateStruct;
  DateStruct.WeekDay = 3;
  DateStruct.Month = 3;
  DateStruct.Date = 22;
  DateStruct.Year = 2019;
  M5.Rtc.SetData(&DateStruct);

  RTC_TimeTypeDef TimeStruct;
  TimeStruct.Hours   = 18;
  TimeStruct.Minutes = 56;
  TimeStruct.Seconds = 10;
  M5.Rtc.SetTime(&TimeStruct);
*/
  ntpSetup = false;   // 初期化

  return;
}
/*
void Rtccont::gettime(RTC_TimeTypeDef *TimeStruct,RTC_DateTypeDef *DateStruct)
{
//  portENTER_CRITICAL(&mutex_rtc);
  M5.Rtc.GetTime(TimeStruct);  //Gets the time in the real-time clock.
  M5.Rtc.GetData(DateStruct);
//  portEXIT_CRITICAL(&mutex_rtc);

  return;
}
*/
uint8_t RtcCont::timeRead(struct tm *timeInfo)
{
/*
  if(deviceChk.rtc() != true){
//    Serial.println("Couldn't find RTC");
    time_t now;
    time(&now);
    localtime_r(&now, timeInfo);
//    getLocalTime(timeInfo,500);
    timeInfo->tm_year = 1900 + timeInfo->tm_year;
    timeInfo->tm_mon += 1;
    return false;
  }
*/

  RTC_TimeTypeDef TimeStruct;
  RTC_DateTypeDef DateStruct;

  M5.Rtc.GetTime(&TimeStruct);  //Gets the time in the real-time clock.
  M5.Rtc.GetData(&DateStruct);

  timeInfo->tm_year = DateStruct.Year - 1900;
  timeInfo->tm_mon = DateStruct.Month - 1;
  timeInfo->tm_mday = DateStruct.Date;
  timeInfo->tm_hour = TimeStruct.Hours;
  timeInfo->tm_min = TimeStruct.Minutes;
  timeInfo->tm_sec = TimeStruct.Seconds;

  char txt[50];//文字格納用
/*
  sprintf(txt, "Data : %04d-%02d-%02d",DateStruct.Year, DateStruct.Month,DateStruct.Date);
  Serial.println(txt);
  sprintf(txt, "Week : %d",DateStruct.WeekDay);
  Serial.println(txt);
  sprintf(txt, "Time : %02d : %02d : %02d",TimeStruct.Hours, TimeStruct.Minutes, TimeStruct.Seconds);
  Serial.println(txt);
*/
/*
  sprintf(txt, "RtcCont::timeRead: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)timeInfo->tm_year), timeInfo->tm_mon, timeInfo->tm_mday,
          timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);//人間が読める形式に変換
  Serial.println(txt);
*/
  return true;
}


void RtcCont::timeSync(struct tm time)
{
  Serial.println("RtcCont::timeSync");

/*
  char txt[50];//文字格納用

  sprintf(txt, "RtcCont::timeSync: %04d/%02d/%02d %02d:%02d:%02d",
          time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
          time.tm_hour, time.tm_min, time.tm_sec);//人間が読める形式に変換
  Serial.println(txt);

  RTC_TimeTypeDef TimeStruct;
  TimeStruct.Hours   = time.tm_hour;  //Set the time.  设置时间
  TimeStruct.Minutes = time.tm_min;
  TimeStruct.Seconds = time.tm_sec;
  RTC_DateTypeDef DateStruct;
  DateStruct.Month = time.tm_mon+1;
  DateStruct.Date = time.tm_mday;
  DateStruct.Year = time.tm_year+1900;// + 1900 - 2000 + 30;

Serial.println("1:");
//  portENTER_CRITICAL(&mutex_rtc);
  M5.Rtc.SetTime(&TimeStruct);  //writes the set time to the real time clock.  将设置的时间写入实时时钟
Serial.println("2:");
  M5.Rtc.SetData(&DateStruct);  //writes the set date to the real time clock.  将设置的日期写入实时时钟
//  portEXIT_CRITICAL(&mutex_rtc);
Serial.println("3:");

    // testtest
    struct tm timeInfo;
    timeRead(&timeInfo);
    timeDisp(timeInfo);
    // testtest
*/
    struct tm timeInfo;

    getLocalTime(&timeInfo);

    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Hours   = timeInfo.tm_hour;
    TimeStruct.Minutes = timeInfo.tm_min;
    TimeStruct.Seconds = timeInfo.tm_sec;
    M5.Rtc.SetTime(&TimeStruct);
    RTC_DateTypeDef DateStruct;
    DateStruct.WeekDay = timeInfo.tm_wday;
    DateStruct.Month = timeInfo.tm_mon + 1;
    DateStruct.Date = timeInfo.tm_mday;
    DateStruct.Year = timeInfo.tm_year + 1900;
    M5.Rtc.SetData(&DateStruct);
/*
    Serial.println(DateStruct.Year);
    Serial.println(DateStruct.Month);
    Serial.println(DateStruct.Date);
    Serial.println(TimeStruct.Hours);
    Serial.println(TimeStruct.Minutes);
    Serial.println(TimeStruct.Seconds);
*/
    timeRead(&timeInfo);
    timeDisp(timeInfo);

  return;
}

void RtcCont::timeDisp(struct tm timeInfo)
{
  char txt[50];//文字格納用
  sprintf(txt, "RtcCont::timeDisp: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)timeInfo.tm_year+1900), timeInfo.tm_mon+1, timeInfo.tm_mday,
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);//人間が読める形式に変換
  Serial.println(txt);

  return;
}
#else

/**
 * @brief 曜日表示用テキスト
 * 
 */
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/*
volatile byte state = LOW;
void IRAM_ATTR onButton() {
  state = !state;
  Serial.print("onButton:");
  Serial.println(state);
}
*/

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


/**
 * @brief Construct a new Rtc Cont:: Rtc Cont object
 * 
 */
RtcCont::RtcCont(void)
{
//  init();

//  Wire.setPins(26,25);
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

//  rtc.writeSqwPinMode(DS1307_SquareWave32kHz);  // 32kHz square wave
  rtc.writeSqwPinMode(DS1307_SquareWave1HZ);  // 1Hz square wave
  
  hzCount = 0;
  pinMode(RTC_PLS_GPIO, INPUT);
//  attachInterrupt(RTC_PLS_GPIO, onButton, FALLING);
  attachInterrupt(RTC_PLS_GPIO, rtcPlusCount, FALLING);


  ntpSetup = false;   // 初期化


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
 * @brief 初期化
 * 
 */
/*
void RtcCont::init(void)
{
//  Wire.setPins(26,25);
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

//  rtc.writeSqwPinMode(DS1307_SquareWave32kHz);  // 32kHz square wave
  rtc.writeSqwPinMode(DS1307_SquareWave1HZ);  // 1Hz square wave
  
  hzCount = 0;
  pinMode(RTC_PLS_GPIO, INPUT);
//  attachInterrupt(RTC_PLS_GPIO, onButton, FALLING);
  attachInterrupt(RTC_PLS_GPIO, rtcPlusCount, FALLING);


  ntpSetup = false;   // 初期化

  return;
}
*/
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
//    getLocalTime(timeInfo,500);
//    timeInfo->tm_year = 1900 + timeInfo->tm_year;
//    timeInfo->tm_mon += 1;
    return false;
  }

  DateTime now = rtc.now();
  if(now.year() == 2000U){
    ret = false;
  }

  timeInfo->tm_year = now.year() - 1900;
  timeInfo->tm_mon = now.month() - 1;
  timeInfo->tm_mday = now.day();
  timeInfo->tm_hour = now.hour();
  timeInfo->tm_min = now.minute();
  timeInfo->tm_sec = now.second();
/*
  char txt[50];//文字格納用
  sprintf(txt, "RtcCont::timeRead: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)timeInfo->tm_year), timeInfo->tm_mon, timeInfo->tm_mday,
          timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);//人間が読める形式に変換
  Serial.println(txt);
*/
  return ret;
}

/**
 * @brief RTCに時刻を設定する
 * 
 * @param timeInfo 設定時刻
 */
void RtcCont::timeSync(struct tm timeInfo)
{
//  struct tm tmRead;
//  char txt[25];//文字格納用

  Serial.println("-- RtcCont::timeSync --");
  if(deviceChk.rtc() != true){
    Serial.println("Couldn't find RTC");
    return;
  }

//  timeRead(&tmRead);   // RTC Read
  /*
  sprintf(txt, "RTC: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)tmRead.tm_year), tmRead.tm_mon, tmRead.tm_mday,
          tmRead.tm_hour, tmRead.tm_min, tmRead.tm_sec);  //人間が読める形式に変換
  Serial.println(txt);
  */
/*
  sprintf(txt, "NTP: %04d/%02d/%02d %02d:%02d:%02d",
          timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);  //人間が読める形式に変換
  Serial.println(txt);
*/
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");

  rtc.adjust(DateTime(timeInfo.tm_year % 100, timeInfo.tm_mon+1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec));

  Serial.println("RtcCont::timeSync : Conplete");
  return;
}

/**
 * @brief 時刻を表示する。（クラスでは無く、汎用にすべき
 * 
 * @param timeInfo 表示する時刻情報
 */
/*
void RtcCont::timeDisp(struct tm timeInfo)
{
  char txt[25];//文字格納用
  sprintf(txt, "RTC: %04d/%02d/%02d %02d:%02d:%02d",
          ((uint16_t)timeInfo.tm_year), timeInfo.tm_mon, timeInfo.tm_mday,
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);//人間が読める形式に変換
  Serial.println(txt);

  return;
}
*/


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


#endif
