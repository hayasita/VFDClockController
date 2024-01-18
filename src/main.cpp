/**
 * @file main.cpp
 * @author hayasita04@gmail.com
 * @brief VFD Clock Controller main
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <M5Unified.h>
#include <Wire.h>

#include "jsdata.h"
#include "vfd_conf.h"
#include "vfd_disp.h"
#include "vfd_driver.h"
#include "vfd_rtc.h"
#include "sound.h"
#include "vfd_web.h"
#include "vfd_wificnt.h"

#include "vfd_eerom.h"
#include "mode_ctrl.h"


// an IR detector/demodulator is connected to GPIO pin 2
#ifdef USE_IR
IRrecv irrecv(IRRCV_PIN);
decode_results results;
#endif

unsigned long rtcAutoUpdateTime;  // Loop内実行タイマ
unsigned long sntpAutoConSqTime;  // SNTP取得自動接続シーケンスタイマ

// Mailbox用のキュー web用センサ情報 taskDisplayCtrl() -> loop()
QueueHandle_t xQueueSensData1;    // taskDeviceCtrl() -> taskDisplayCtrl()
QueueHandle_t xQueueSensData2;    // taskDeviceCtrl() -> loop()

/**
 * @brief タスク間通信用構造体
 * 
 */
struct mailboxData{
  uint16_t temp;
  uint16_t humi;
  uint16_t pressure;
  struct tm timeInfo;
  uint16_t illumiData;
  uint16_t dcdcTrg;
  uint16_t dcdcFdb;
  uint16_t Data0;
  uint16_t Data1;

  // Display Mode Ctrl
  uint8_t ctrlMode;           // 操作モード
  uint8_t dispModeVfd;        // VFD表示モード
  uint8_t dispModeM5OLED;     // M5OLED表示モード
  uint8_t dispModeOLED;       // OLED表示モード

};

struct i2cStartData{
  // Display Mode Ctrl
  bool ssd1306Valid;          // OLED有無
  bool m5oledValid;           // M5OLED有無
  struct tm rtcTimeInfo;      // RTC時刻
};

// イベント送信キュー(Mailboxとは別) taskDeviceCtrl() -> loop()
QueueHandle_t xQueueEvent;
#define QUEUE_LENGTH 4

// RTC情報送信キュー taskDeviceCtrl() -> taskDisplayCtrl()
QueueHandle_t xQueueRtcData;

// sysTime送信キュー
QueueHandle_t xQueueSysTimeData1;   // taskDisplayCtrl() -> taskDeviceCtrl()
QueueHandle_t xQueueSysTimeData2;   // taskDisplayCtrl() -> loop()

// mailbox作成
struct mailboxData mailboxDat;
// キュー作成
struct eventQueData eventQueDat;

void IRAM_ATTR disp_vfd_iv21(void);
void IRAM_ATTR vfd_data_trans(uint8_t);
void IRAM_ATTR onTimer(void);

hw_timer_t *timer = NULL;

unsigned long loopTime,loopTimeMax=0,loopTimeLast;
unsigned long taskDeviceTime,taskDeviceTimeMax=0,taskDeviceTimeLast;
unsigned long taskDeviceTime0,taskDeviceTimeMax0=0,taskDeviceTimeLast0;
unsigned long taskDeviceTime1,taskDeviceTimeMax1=0,taskDeviceTimeLast1;
unsigned long taskDeviceTime2,taskDeviceTimeMax2=0,taskDeviceTimeLast2;
unsigned long taskDeviceTime3,taskDeviceTimeMax3=0,taskDeviceTimeLast3;

unsigned long timerExecTime,timerExecTimeMax=0,timerExecTimeLast;
unsigned long timerScanTime,timerScanTimeMax=0,timerScanTimeLast;


// ----- VFD,LED表示処理タスク
// ----- デバイス制御タスク
#define DCDC_SCAN_TIME    50
#define RTC_SCAN_TIME     100
#define ILLUMI_SCAN_TIME  500
extern const struct I2CEEROM_DATA eeromList[];

// テスト用タスク CPU0 
// 最終的には削除する。
TaskHandle_t taskHandle;
void testTask(void *pvParameters) {
  uint32_t ulNotifiedValue;
  while (1) {
    // 通知が来るまで待機する。値のクリアはしない
    xTaskNotifyWait( 0,
                     0,
                     &ulNotifiedValue,
                     portMAX_DELAY );
    Serial.println( "-- testTask" );

//    Serial.println( pcTaskGetTaskName(NULL) );
//    Serial.println( ulNotifiedValue );
//    settingjsFile.writeJsonFile();  // jsonファイル出力

  }
}

// 時刻表示タスク
TaskHandle_t taskDisplayHandle;
void taskDisplayCtrl(void *pvParameters) {
  BaseType_t ret;

  i2cStartData i2cStartDat;       // 起動時RTC,i2c情報
//  struct tm rtcTimeInfo;
  static uint8_t lastSecw;        //  前回秒
  unsigned long timetmp;          // millis()tmp
  unsigned long illumiLasttime;   // 照度読み込み前回時間(millis)
  unsigned long dcdcLasttime;     // DCDCコンバータ処理前回時間(millis)

  uint8_t glowInTheBright;        // 全体輝度設定値：明情報
  uint8_t glowInTheDark;          // 全体輝度設定値：暗情報
  int dcdcVBright;                // 全体輝度明：目標電圧
  int dcdcVDark;                  // 全体輝度暗：目標電圧
  int dcdcV;                      // DCDCコンバータ目標電圧

  uint8_t keydata;                // キー入力情報

  struct mailboxData mailboxDispDat;
  struct mailboxData mailboxDat2Loop;   // Loop送信用mailbox

  Serial.println("+++ taskDisplayCtrl +++");

  // Brightnes Sensor Setup
  illumi.ini(ILLUMI_PIN);

  // IR Controler SetUp
#ifdef USE_IR
  Serial.println("IR Recv Setup.");
  irrecv.enableIRIn(false);  // Start the receiver
#endif

#ifdef USE_DCDC
  Dcdccont dcdcContrl;
  // DCDC Converter init
  dcdcContrl.ini(DCDCFDB_PIN, DCDCPWM_PIN,DCDC_TargetV);
#endif

  // Display Setting
  DISPLAY_DATA dispInputData;
#ifdef USE_VFD
  DispCtr dispVFD;

  // VFD表示　排他処理初期化
  timerMux = portMUX_INITIALIZER_UNLOCKED;

  // VFD init
  dispVFD.init();
  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);
  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 50, true);
  // Start an alarm
  timerAlarmEnable(timer);
#endif

  // taskDeviceCtrl()から時刻受信
  do{
//    ret = xQueueReceive(xQueueRtcData, &rtcTimeInfo, 0);
    ret = xQueueReceive(xQueueRtcData, &i2cStartDat, 0);
  }while(!ret);
  Serial.print("xQueueRtcData:");
//  Serial.print(rtcTimeInfo.tm_hour);
//  Serial.print(rtcTimeInfo.tm_min);
//  Serial.println(rtcTimeInfo.tm_sec);
  Serial.print(i2cStartDat.rtcTimeInfo.tm_hour);
  Serial.print(i2cStartDat.rtcTimeInfo.tm_min);
  Serial.println(i2cStartDat.rtcTimeInfo.tm_sec);
  Serial.print("deviceChk.ssd1306():");
  Serial.println(i2cStartDat.ssd1306Valid);
  Serial.print("deviceChk.m5oled():");
  Serial.println(i2cStartDat.m5oledValid);

  // システム時刻初期化
  SystemTimeCont sysTimCnt;           // システム時刻管理
  sysTimCnt.init(&i2cStartDat.rtcTimeInfo);       // システム時刻初期化
  sysTimCnt.read();                   // システム時刻読み出し
  lastSecw = i2cStartDat.rtcTimeInfo.tm_sec;

  mailboxDat2Loop.Data0 = 0;
  mailboxDat2Loop.Data1 = 0;
  mailboxDat2Loop.dcdcFdb = 0;
  mailboxDat2Loop.dcdcTrg = 0;
  mailboxDat2Loop.humi = 0;
  mailboxDat2Loop.illumiData = 0;
  mailboxDat2Loop.pressure = 0;
  mailboxDat2Loop.temp = 0;
  mailboxDat2Loop.timeInfo = i2cStartDat.rtcTimeInfo;

  // 内部タイマ初期化
  dcdcLasttime = millis();
  illumiLasttime = dcdcLasttime;

  modeCtrl vfdModeCtrl(i2cStartDat.ssd1306Valid,i2cStartDat.m5oledValid);

  while (1) {
    struct tm *sysTimeInfo;
//    struct tm rtcTimeInfo;

    timetmp = millis();

    // RTC時刻受信
    ret = xQueueReceive(xQueueRtcData, &i2cStartDat, 0);
    if(ret){
      sysTimCnt.write(i2cStartDat.rtcTimeInfo);       // システム時刻設定
    }

    // RTCパルス割り込みで計時
    if (xSemaphoreTake(rtcPalsSemaphore, 0) == pdTRUE) {
      sysTimCnt.timeAdd();
      time_t sysTimeTmp = sysTimCnt.read();
      sysTimeInfo = localtime(&sysTimeTmp);
//      Serial.println(sysTimeInfo->tm_sec);
      mailboxDat2Loop.timeInfo = *sysTimeInfo;
      ret = xQueueOverwrite(xQueueSysTimeData1, &mailboxDat2Loop);   // taskDeviceCtrl()へシステム時刻を送信
      ret = xQueueOverwrite(xQueueSysTimeData2, &mailboxDat2Loop);   // loop()へシステム時刻を送信
    }

    // DCDC Control
    if(timetmp - dcdcLasttime > DCDC_SCAN_TIME){    // 50mSecごとに実行

#ifdef USE_DCDC
      // DCDCコンバータ制御
      dcdcLasttime = timetmp;
      dcdcContrl.ctr(dcdcV);
      mailboxDat2Loop.dcdcFdb = dcdcContrl.getaddat();
      mailboxDat2Loop.dcdcTrg = dcdcV;
#endif

    }
    // 外光取込・DCDC目標値作成
    if(timetmp - illumiLasttime >ILLUMI_SCAN_TIME){   // 500mSecごとに実行
      illumiLasttime = timetmp;

      dcdcV = dcdcContrl.getTarget(illumi.read());      // DCDC目標値を外光取込値から作成
      mailboxDat2Loop.illumiData = illumi.getaddat();   // 外光取込値をタスク間通信用データに積む
    }

    // LED表示マネージャ
    led.ledMan();

#ifdef USE_IR
    if (irrecv.decode(&results)) {
      Serial.println("--- irDump ---");
      irDump(&results);
//      Serial.println("DEPRECATED: Please use IRrecvDumpV2.ino instead!");
      irrecv.resume();  // Receive the next value
    }
#endif

    // 端子入力
    keydata = itmMan();

    // 操作モード更新
    vfdModeCtrl.modeSet(keydata);
//    mailboxDat2Loop.ctrlMode = vfdModeCtrl.getCtrlMode();
//    mailboxDat2Loop.dispModeVfd = vfdModeCtrl.getDispModeVfd();
//    mailboxDat2Loop.dispModeOLED = vfdModeCtrl.getDispModeOLED();
//    mailboxDat2Loop.dispModeM5OLED = vfdModeCtrl.getDispModeM5OLED();

    // センサ情報受信
    ret = xQueueReceive(xQueueSensData1, &mailboxDispDat, 0);
    if(ret){
      dispInputData.temp = mailboxDispDat.temp;
      dispInputData.humi = mailboxDispDat.humi;
      dispInputData.pressure = mailboxDispDat.pressure;
    }

#ifdef USE_VFD
    dispVFD.dispModeSet(keydata);           // 表示モード設定

    dispInputData.timeInfo = *sysTimeInfo;  // 表示用時刻情報設定
    dispVFD.dataMake(dispInputData);        // VFD表示データ作成
#endif

    if(lastSecw != sysTimeInfo->tm_sec){     // 毎秒ごとに実行
      lastSecw = sysTimeInfo->tm_sec;

    }

    delay(1);
  }
}

/**
 * @brief i2cデバイスの制御タスク
 * 
 * @param Parameters 
 * @details i2c接続デバイスの初期設定完了後にループでデバイスの制御を行う。
 */
void taskDeviceCtrl(void *Parameters){
  unsigned long timetmp;      // millis()tmp

  static uint8_t lastSecw;        // 前回秒
  unsigned long rtcLasttime;      // RTC読み込み前回時間(millis)
  unsigned long illumiLasttime;   // 照度読み込み前回時間(millis)
  unsigned long sensor2Lasttime;  // センサスキャン処理前回時間(millis)

  DeviceData i2cDeviceData;       // i2c接続デバイス情報 /***** i2cではなく、センサデータでは？

  i2cStartData i2cStartDat;       // 起動時RTC,i2c情報
//  struct tm rtcTimeInfo;
  struct tm *sysTimeInfo;

  BaseType_t ret;

  // == I2C Device init ==
  Wire.begin(SDA_PIN,SCL_PIN);

  // I2C Device Check
  deviceChk.i2cScan();

  i2cStartDat.ssd1306Valid = deviceChk.ssd1306();
  i2cStartDat.m5oledValid = deviceChk.m5oled();
  // RTC control instance
  RtcCont RtcContrl;

  // M5ENVIII Sensor Init
  SensorEnviii enviii;
  if(deviceChk.sht30() && deviceChk.qmp6988()){
    enviii.init();     // xQueueMailboxの処理よりも下でないと失敗する。？？
  }

  // bme680 Sensor Init
  SensorBme680 bme680;
  if(deviceChk.bme680()){
    bme680.init(&i2cDeviceData.bme680Data);
  }

  // OLED Display
  OLEDDISP oledDisp;
  if(deviceChk.ssd1306()){
    oledDisp.init();
  }

//  M5UnitOLED oled;                // QMP6988より下でなければならない？？
  M5OLED m5Oled;
  if(deviceChk.m5oled()){
    m5Oled.init();
  }

  // == システム時刻初期化 ==
//  RtcContrl.timeRead(&rtcTimeInfo);   // RTC 時刻読み込み
//  lastSecw = rtcTimeInfo.tm_sec;      // 前回秒 設定
//  sysTimeInfo = &rtcTimeInfo;         // 仮初期化
  RtcContrl.timeRead(&i2cStartDat.rtcTimeInfo);   // RTC 時刻読み込み
  lastSecw = i2cStartDat.rtcTimeInfo.tm_sec;      // 前回秒 設定
  sysTimeInfo = &i2cStartDat.rtcTimeInfo;         // 仮初期化

  // RTC時刻をtaskDisplayCtrlへ通知
  // この通知でtaskDisplayCtrl処理開始するので、タイミング注意
//  xQueueOverwrite(xQueueRtcData, &rtcTimeInfo);
  xQueueOverwrite(xQueueRtcData, &i2cStartDat);

  vfdevent.setEventlogDeviceCtrl(EVENT_BOOT_TASKDEVICECTRL);   // 起動

  // 起動時SNTP自動接続要求
  ntpAutoSetreq();
//  mailboxDat.lastUpdateSend = OFF;

  // 内部タイマ初期化
  rtcLasttime = millis();
  illumiLasttime = rtcLasttime;
  sensor2Lasttime = rtcLasttime;

  confDat.debugI2cWritePr();      // Debug
  Serial.println("-- taskDeviceCtrl Start --");

// eerom.sequentialWrite Test
/*
  getLocalTime(&timeInfo);
  uint8_t testdata[16];
  testdata[0] = timeInfo.tm_mon;
  testdata[1] = timeInfo.tm_mday;
  testdata[2] = timeInfo.tm_hour;
  testdata[3] = timeInfo.tm_min;
  testdata[4] = timeInfo.tm_sec;
  testdata[5] = 0x01;
  testdata[6] = 0x02;
  testdata[7] = 0x03;
  testdata[8] = timeInfo.tm_mon;
  testdata[9] = timeInfo.tm_mday;
  testdata[10] = timeInfo.tm_hour;
  testdata[11] = timeInfo.tm_min;
  testdata[12] = timeInfo.tm_sec;
  testdata[13] = 0x01;
  testdata[14] = 0x02;
  testdata[15] = 0x03;
  eerom.sequentialWrite(0x50,ADR_eventLogStart,testdata,16);
  eerom.dump(ADR_eventLogStart,ADR_eventLogStart+16);
*/
// 初期化
//  confDat.setEeromEventStartNum(0);
//  confDat.setEeromEventWriteNum(0);
  Serial.print("/-/- Heap available : ");
  Serial.println(esp_get_free_heap_size());

  //OLED 画面クリア
  if(deviceChk.ssd1306()){
    oledDisp.clear();
  }

  // M5OLED 画面クリア
  if(deviceChk.m5oled()){
    m5Oled.clear();
  }

  while(1){
    struct tm tmpTimeInfo;            // 時刻処理用Tmp
    struct mailboxData mailboxDisplayCtrl;

    taskDeviceTimeLast = micros();    // 実行時間測定用基本時刻
    timetmp = millis();               // 処理間隔確認用基本時刻

    // システム時刻受信
    ret = xQueueReceive(xQueueSysTimeData1, &mailboxDisplayCtrl, 0);   // taskDisplayCtrl()からシステム時刻情報を受信
    if(ret){
      *sysTimeInfo = mailboxDisplayCtrl.timeInfo;       // システム時刻設定
      debugData.timeInfo = mailboxDisplayCtrl.timeInfo;
      debugData.deviceDat.dcdcTrg = mailboxDisplayCtrl.dcdcTrg;
      debugData.deviceDat.dcdcFdb = mailboxDisplayCtrl.dcdcFdb;
      debugData.deviceDat.illumiData = mailboxDisplayCtrl.illumiData;
//      debugData.dcdcTrg = mailboxDisplayCtrl.dcdcTrg;
//      debugData.dcdcFdb = mailboxDisplayCtrl.dcdcFdb;
//      debugData.illumiData = mailboxDisplayCtrl.illumiData;
//      Serial.print("syst:");
//      Serial.println(mailboxDisplayCtrl.illumiData);
    }


    if(timetmp - illumiLasttime >ILLUMI_SCAN_TIME){   // 500mSecごとに実行
      illumiLasttime = timetmp;

      mailboxDat.illumiData = 0;   // Debug

      // OLED表示データ作成
      RtcContrl.timeRead(&i2cStartDat.rtcTimeInfo);   // RTC 時刻読み込み
      debugData.rtcTimeInfo = i2cStartDat.rtcTimeInfo;
      debugData.deviceDat.bme680Data = i2cDeviceData.bme680Data;

      //OLED 画面表示
      if(deviceChk.ssd1306()){
//        oledDisp.printEnvSensorData(debugData);
//        oledDisp.printEventLog(debugData);
//        i2cDevice i2cDeviceDat;
        i2cDeviceDat.datSSD1306 = deviceChk.ssd1306();
        i2cDeviceDat.datM5OLED = deviceChk.m5oled();
        oledDisp.printDeviceData(i2cDeviceDat);
      }

      // M5OLED 画面表示 
      if(deviceChk.m5oled()){
        m5Oled.printEnvSensorData(debugData);
      }

      // 処理時間測定
      taskDeviceTime2 = micros() - taskDeviceTimeLast;
      taskDeviceTimeMax2 = (taskDeviceTimeMax2 > taskDeviceTime2) ? taskDeviceTimeMax2 : taskDeviceTime2;
    }

    if(timetmp - sensor2Lasttime >2000){   // 2000mSecごとに実行
      sensor2Lasttime = timetmp;
    
      // ENVIII Seneorデータ取得
      if(deviceChk.sht30() && deviceChk.qmp6988()){
        enviii.read(&debugData.deviceDat.enviiiData);
        mailboxDat.temp = (uint16_t)(debugData.deviceDat.enviiiData.env3Temperature * 10);
        mailboxDat.humi = (uint16_t)(debugData.deviceDat.enviiiData.env3Humidity * 10);
        mailboxDat.pressure = (uint16_t)(debugData.deviceDat.enviiiData.env3Pressure);
      }

      // BME680 Data データ取得
      if(deviceChk.bme680()){
        bme680.read(&i2cDeviceData.bme680Data);
      }


      ret = xQueueOverwrite(xQueueSensData1, &mailboxDat);      // taskDisplayCtrl()へセンサ情報を送信
      ret = xQueueOverwrite(xQueueSensData2, &mailboxDat);      // loop()へセンサ情報を送信
    }

    // Callbackイベント要求処理

    // SNTP処理要求イベント要求
    if(eventAutoConSntpReq){
      eventAutoConSntpReq = false;
      vfdevent.setEventlogDeviceCtrl(EVENT_WIFI_AUTOCON_SNTP_REQ);   // SNTP処理要求
    }

    // SNTP同期完了イベント要求
    if(eventSntpSync){
      eventSntpSync = false;
      vfdevent.setEventlogDeviceCtrl(EVENT_WIFI_AUTOCON_SNTP_COMP);   // SNTP処理完了
    }

    // NTP時刻→RTC設定・システム時刻設定
    // RTC設定要求対応処理
    if(ntpSetup){
      struct tm timeInfo;
      Serial.println("-- NTP Setup!! --");
      ntpSetup = false;
      getLocalTime(&timeInfo);            // localtime(NTP反映済み)読み出し
      RtcContrl.timeSync(timeInfo);       // RTCへ時刻設定
//      sysTimCnt.write(timeInfo);          // システム時刻情報設定

//      struct tm TimeInfoTest;
//      RtcContrl.timeRead(&TimeInfoTest);  // 時刻読み込み
      RtcContrl.timeRead(&i2cStartDat.rtcTimeInfo);  // 時刻読み込み
      uint16_t data[4];
      data[0] = (uint16_t)i2cStartDat.rtcTimeInfo.tm_min;
      data[1] = (uint16_t)i2cStartDat.rtcTimeInfo.tm_sec;
      data[2] = (uint16_t)i2cStartDat.rtcTimeInfo.tm_min;
      data[3] = (uint16_t)i2cStartDat.rtcTimeInfo.tm_sec;

      xQueueOverwrite(xQueueRtcData, &i2cStartDat);          // 設定時刻をシステム時刻へ通信
//      confDat.eeromConfWrite();         // E2ROM設定
      vfdevent.setEventlogDeviceCtrl(EVENT_RTCSET,data);
    }

    // タスク間通信処理
    if(uxQueueSpacesAvailable(xQueueEvent) != 0){             // キューの追加可能数が0ではない
      uint8_t eventnum = vfdevent.eventSend(&eventQueDat);    // websocket有効ならばイベントLogをEEPROMとバッファから読み出す
      if(eventnum != 0){
//        Serial.println("-- uxQueueSpacesAvailable--");
//        for(uint8_t i=0;i<eventnum;i++){
//          vfdevent.putLog(eventQueDat.eventData[i]);
//        }

        eventQueDat.dataSize = eventnum;                  // 送信データ数設定
        xQueueSend(xQueueEvent, &eventQueDat, 0);         // loop()へイベント情報を送信
      }
    }

    // 経過時間測定
    taskDeviceTime3 = micros() - taskDeviceTimeLast;
    taskDeviceTimeMax3 = (taskDeviceTimeMax3 > taskDeviceTime3) ? taskDeviceTimeMax3: taskDeviceTime3;

    // イベントLogバッファからEEPROMへ書き込み
    vfdevent.eventBuf2Eeprom();

    //i2c EEROM WriteReq
    confDat.i2cEepromWrite();             // i2cEEPROM書込処理

    taskDeviceTime = micros() - taskDeviceTimeLast;
    taskDeviceTimeMax = (taskDeviceTimeMax > taskDeviceTime) ? taskDeviceTimeMax : taskDeviceTime;

    delay(1);
  }

}

void setup(void){
  // Initialize the M5Stack object
  auto cfg = M5.config();
  cfg.external_speaker.hat_spk2 = true;   // I2Sサウンド出力に必要
  M5.begin(cfg);
  M5.In_I2C.release();

  // Serial Setting
  Serial.begin(115200);


  unsigned long startWaitTime;
  startWaitTime = millis();
  while(1){
    M5.update();
    if((millis() - startWaitTime > (unsigned long)20000) || (Serial.available() > 0) || (M5.BtnA.wasReleased()) ){ // 20Sec or space input
      // データを読み取り
      char data = Serial.read();
      break;
    }
  };


  Serial.println("\nIV-21 Clock System Starting.");

  // -- i2c Init --
  Wire.begin(SDA_PIN,SCL_PIN);

  // -- ConfigData Setting --
//  deviceChk.init();               // i2C Device chk
  confDat.init();
//  eerom.conectChk();
//  eerom.allclear();               // E2ROMデータクリア
  confDat.setConfigDefault();     // ConfigData 初期値設定
  confDat.i2cEepromRead();        // ConfigData i2cEEPROM値読み出し
  confDat.eeromDataout();

  Wire.end();

  // I2S Sound setup
  i2sS3Sound.init();

  //SPIFFS Open
  uint8_t timeOut = 0;
  while (false == SPIFFS.begin()) {
    if(timeOut++ > 6) return;
    Serial.println("SPIFFS Wait...");
//    M5.Lcd.println("SD Wait...");
    delay(500);
  }

  // -- Clock Setting --
  // システムクロック初期化
  //localTimeCont.init();

  // RTCパルス計時要求バイナリセマフォ作成
  rtcPalsSemaphore = xSemaphoreCreateBinary();

  // -- WiFi Setting --
  setWiFiCallBack();    // Set WiFi Call Back.

  setWebhandle();       // Web handle setting

  // -- Task Control ---

  // タスク間通信のキューを初期化
  xQueueSensData1 = xQueueCreate(1, sizeof(mailboxData));
  xQueueSensData2 = xQueueCreate(1, sizeof(mailboxData));

  xQueueEvent = xQueueCreate(QUEUE_LENGTH, sizeof(eventQueData));
  xQueueRtcData = xQueueCreate(1, sizeof(tm));
  xQueueSysTimeData1 = xQueueCreate(1, sizeof(mailboxData));
  xQueueSysTimeData2 = xQueueCreate(1, sizeof(mailboxData));

  // Core1で関数taskDeviceCtrlをstackサイズ4096,優先順位1で起動
  xTaskCreatePinnedToCore(taskDeviceCtrl, "taskDeviceCtrl", 4096, NULL, 1, NULL, 1);

  // Core1で関数taskDisplayCtrlをstackサイズ4096,優先順位1で起動
  xTaskCreatePinnedToCore(taskDisplayCtrl, "taskDisplayCtrl", 4096, NULL, 1, &taskDisplayHandle, 1);

  // Core1で関数taskSoundCtrlをstackサイズ4096,優先順位1で起動
  xTaskCreatePinnedToCore(taskSoundCtrl,"taskSoundCtrl",4096,NULL,1,&sountaskHandle,1);

  // Core0でタスク起動 テストタスク
//  xTaskCreatePinnedToCore(testTask,"loopTask1",8192,NULL,1,&taskHandle,0);

  // 起動音ピポッ
  i2sS3Sound.soundPipo();

  // -- Flug Setup --
  getwifiStaListreq = 0;          // WiFiStationList取得初期化
  confWiFiStartf = false;         // WiFi手動接続要求初期化
//  settingjsFileWriteReq = 0;    // SPTFFS Setting.js作成要求初期化
  rtcAutoUpdateTime = millis();   // Loop内実行タイマ初期化

  Serial.println("-- Setup OK --");
}

void loop(void){
  struct mailboxData mailboxDat;
  struct mailboxData mailboxDisplayCtrl;

  static bool mailboxStart = 0;
  static  uint8_t wifiStatus;       // WiFi接続状態(WiFiClass)
  BaseType_t ret;
  bool wifiOn = OFF;                // WiFi接続ON/OFF

  struct tm tmpTimeInfo;            // 時刻処理用Tmo
  struct tm sysTimeInfo;
  BaseType_t retSysTime;


  loopTimeLast = micros();

  // WiFi接続制御
  if(mailboxStart == 1){  // mailbox start == RTC初期化完了
    wifiOn = wifiConnectMan();
  }

  // WiFi接続状態更新
  if(wifiStatus != WiFi.status()){
    wifiStatus = WiFi.status();
    Serial.print("-= wifiStatus : ");
    Serial.println(wifiStatus);
  }

  if(wifiOn == ON){   // Wifi接続時実行
  // webサーバ処理
    webHandleClient();
  }

  // 100msごとに実行
  if(millis() - rtcAutoUpdateTime > 100){
    rtcAutoUpdateTime = millis();

    // WiFiStationList作成 todo. classにしてvfd_web.cppに移動
    if(getwifiStaListreq == 1){   // WiFiStationList取得要求
      int16_t ssidNum;
      if(getwifiListsqf == 0){
        vfdevent.setEventlogLoop(EVENT_WiFi_SSIDSCAN_START);
        getwifiListsqf = 1;
      }
      else if(getwifiListsqf == 1){
        Serial.println("scan done\r\n");
        ssidNum = WiFi.scanNetworks(true);
        getwifiListsqf = 2;
      }
      else if(getwifiListsqf == 2){
        ssidNum = WiFi.scanComplete();
        if(ssidNum == WIFI_SCAN_RUNNING){
          Serial.println("WIFI_SCAN_RUNNING\r\n");
        }
//        else if(ssidNum == WiFi_SCAN_FAILED){
          // 失敗
//        }
        else{
          //
          vfdevent.setEventlogLoop(EVENT_WiFi_SSIDSCAN_COMP);     // WiFi SSID 検索完了
          #define SSIDLIMIT 30
          String ssid_rssi_str[SSIDLIMIT];
          String ssid_str[SSIDLIMIT];
          String str = "\"stationList\":[\n";
          if (ssidNum == 0) {
            Serial.println("no networks found");
          } else {
            Serial.printf("%d networks found\r\n\r\n", ssidNum);
            if (ssidNum > SSIDLIMIT) ssidNum = SSIDLIMIT;
            for (int i = 0; i < ssidNum; ++i) {
              ssid_str[i] = WiFi.SSID(i);
              String wifi_auth_open = ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
              ssid_rssi_str[i] = ssid_str[i] + " (" + WiFi.RSSI(i) + "dBm)" + wifi_auth_open;
              if(i != 0){
                str = str + ",\n";
              }
              str = str + "{\"ID\":\"" + ssid_str[i] + "\",\"TITLE\":\"" + ssid_rssi_str[i] + "\"}";
              Serial.printf("%d: %s\r\n", i, ssid_rssi_str[i].c_str());
            }
            str = str + "\n]";
            websocketDataSend.wifiStaList = str;
            websocketDataSend.wifiStaListSend = ON;
            getwifiStaListreq = 0;
          }
        }
      }
    }
    else{
      getwifiListsqf = 0;
    }

    retSysTime = xQueueReceive(xQueueSysTimeData2, &mailboxDisplayCtrl, 0);   // taskDisplayCtrl()からシステム時刻情報を受信
    if(retSysTime){
      sysTimeInfo = mailboxDisplayCtrl.timeInfo;       // システム時刻設定
//      Serial.print("Loop syst:");
//      Serial.println(sysTimeInfo.tm_sec);
      websocketDataSend.timeInfo = sysTimeInfo;
      websocketDataSend.basicDataSend = ON;   // 基本データ送信フラグON

      // SNTP自動接続・更新時刻チェック
      uint8_t tmp1;
      uint8_t tmp2;
      confDat.getAutoUpdatetime(&tmp1,&tmp2);
//      Serial.printf("tm_hour:%d : %d\n",sysTimeInfo.tm_hour,sysTimeInfo.tm_min);
//      Serial.printf("tmp1:%d : %d\n",tmp1,tmp2);
      if( ((uint8_t)sysTimeInfo.tm_hour == tmp1) && ((uint8_t)sysTimeInfo.tm_min == tmp2)){
//        Serial.print("/-/- Heap available NTP自動取得接続時刻確認 : ");
//        Serial.println(esp_get_free_heap_size());
        if(wifiOn != ON){       // WiFi未接続
          if(ntpAutoSetreq()){              // SNTP自動接続要求
            sntpAutoConSqTime = millis();   // SNTP取得自動接続シーケンスタイマ初期化
          }
        }
      }
      else if(millis() - sntpAutoConSqTime > (1000 * 90)){    // 90Sec経過で終了判定実行
      // 自動接続側のタイムアウト処理にまかせたほうがよい。要検討。
        if(ntpAutoSetSqf == RTCAUTO_DISCONNECTION){
          uint16_t data[4];
          data[0] = (uint16_t)sysTimeInfo.tm_hour;
          data[1] = (uint16_t)sysTimeInfo.tm_min;
          data[2] = (uint16_t)tmp1;
          data[3] = (uint16_t)tmp2;
          ntpAutoSetSqf = RTCAUTO_STANDBY;
          vfdevent.setEventlogLoop(EVENT_SNTP_AUTOCON_TIME_CONP,data);   // SNTP取得時刻　接続動作完了
          Serial.println("-- SNTP取得　接続動作完了 --");
        }
      }
    }

    // taskDeviceCtrl()からセンサ情報他を00秒ごとに受信
    ret = xQueueReceive(xQueueSensData2, &mailboxDat, 0);
    if(ret){
//    if(ret & retSysTime){
//      uint8_t tmp1;
//      uint8_t tmp2;

      mailboxStart = 1;
//      Serial.printf("xQueueReceive(%d:%d) : ret = %d\n", mailboxDat.timeInfo.tm_sec, mailboxDat.illumiData, ret);

      // WebSocket Data送信
      if(websocketDataSend.getReady() && websocketConnect){   // websocket通信準備完了
//        websocketDataSend.timeInfo = mailboxDat.timeInfo;
        websocketDataSend.sensorData[0] = mailboxDat.temp;
        websocketDataSend.sensorData[1] = mailboxDat.humi;
        websocketDataSend.sensorData[2] = mailboxDat.pressure;
        websocketDataSend.sensorData[3] = mailboxDisplayCtrl.illumiData;
        websocketDataSend.sensorData[4] = mailboxDisplayCtrl.dcdcFdb;
        websocketDataSend.sensorData[5] = mailboxDisplayCtrl.dcdcTrg;
        websocketDataSend.sensorData[6] = mailboxDat.Data0;
        websocketDataSend.sensorData[7] = mailboxDat.Data1;
//        websocketDataSend.sensorData[7] = mailboxDat.timeInfo.tm_sec;
//        websocketDataSend.basicDataSend = ON;   // 基本データ送信フラグON
      }
    }
    else{
      struct eventQueData eventQueRcvDat;
      // taskDeviceCtrl()からイベント送信キューを受信
      if(websocketConnect){   // websocket通信可能
        uint8_t event;
        event = xQueueReceive(xQueueEvent, &eventQueRcvDat, 0);
        if(event){
          // 受信データあり
//          Serial.println("-- xQueueReceive--");
//          Serial.print("dataNum : ");
//          Serial.println(eventQueRcvDat.dataSize);
//          for(uint8_t i=0;i<eventQueRcvDat.dataSize;i++){
//            vfdevent.putLog(eventQueRcvDat.eventData[i]);
//          }
          websocketDataSend.websocketEventSend(eventQueRcvDat);  // 送信処理
        }
      }
    }

  }

  if(websocketConnect){   // websocket通信可能
    // Websocket Data 送信
    if( websocketDataSend.dataReady()
    /*|| vfdevent.withTransEventData()*/ ){  // イベントログ送信データあり（旧方式）
      websocketDataSend.websocketSend();  // 送信処理
      // 送信処理の内部で送信するイベントLOG取得している。
      // mailboxから取得するように修正する。
    }
  }

  delay(1);

  loopTime = micros() - loopTimeLast;
  loopTimeMax = (loopTimeMax > loopTime) ? loopTimeMax : loopTime;

}

void IRAM_ATTR onTimer(){
//  BaseType_t taskWoken;
//  xTaskNotifyFromISR(taskHandle, 0, eIncrement, &taskWoken);
  portENTER_CRITICAL_ISR(&timerMux);

//  timerExecTimeLast = micros();

  timerScanTime = micros() - timerScanTimeLast;
  timerScanTimeLast = micros();
  timerScanTimeMax = (timerScanTimeMax > timerScanTime) ? timerScanTimeMax : timerScanTime;

  disp_vfd_iv21();

//  timerExecTime = micros() - timerExecTimeLast;
//  timerExecTimeMax = (timerExecTimeMax > timerExecTime) ? timerExecTimeMax : timerExecTime;

  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR vfd_data_trans(uint8_t val)
{
  uint8_t i;
  for (i = 0; i < 8; i++){
    digitalWrite(SPIMOSI_PIN, (val & 128) != 0);
    val <<= 1;
    digitalWrite(SPISCLK_PIN, HIGH);
    digitalWrite(SPISCLK_PIN, LOW);
  }
  return;
}

void IRAM_ATTR disp_vfd_iv21(void)
{
  static unsigned char dispketaw;         // 表示桁
  static unsigned char pwm_countw;        // PWMカウンタ
  static unsigned long lasttime[9];       // クロスフェードカウンタ周期前回値
  static unsigned long fade_cyclel[9];    // クロスフェードサイクルカウンタ更新間隔
  unsigned long dispdata;                 // 表示データ
//  unsigned char brightness_tmpw;                  // 輝度情報オーバーフロー対策
 
  // 点灯する桁更新
  if (dispketaw >= (DISP_KETAMAX - 1)) {
    dispketaw = 0;                        // 表示桁リセット
    pwm_countw++;                         // PWMカウンタ更新
  }
  else {
    dispketaw++;                          // 表示桁更新
  }

  // クロスフェード初期化
  if((disp_last[dispketaw] != disp[dispketaw]) && (disp_fadecount[dispketaw] == 0)){  // 表示データ更新された && クロスフェードサイクルカウンタ未更新
    lasttime[dispketaw] = millis();                               // クロスフェードサイクルカウンタ更新間隔初期化
    fade_cyclel[dispketaw] = disp_fadetimei / brightness_dig[dispketaw];       // クロスフェードサイクルカウンタ更新間隔計算　フェード時間/輝度
    disp_fadecount[dispketaw]++;                                  // クロスフェードサイクルカウンタ更新
  }
  if(disp_fadecount[dispketaw] != 0){
    if(millis() - lasttime[dispketaw] > fade_cyclel[dispketaw]){  // クロスフェードサイクルカウンタ更新時間チェック
      lasttime[dispketaw] = millis();                             // サイクルカウンタ更新前回値更新
      disp_fadecount[dispketaw]++;                                // クロスフェードサイクルカウンタ更新
    }
  }

  // 各桁輝度決定PWM処理
  if ((pwm_countw & 0x0F)  >= brightness_dig[dispketaw] ) {   // PWM処理
    dispdata = font[DISP_NON];                              // PWM消灯
  }else{
    if((pwm_countw & 0x0F) < disp_fadecount[dispketaw]){    // PWM点灯 // クロスフェード今回：前回表示比率判定　暫定でpwm_countwを使用する
      dispdata = disp[dispketaw];                           // 今回データを優先表示
    }else{
      dispdata = disp_last[dispketaw];                      // 前回データ表示
    }
  }

  // クロスフェード終了判定
  if(disp_fadecount[dispketaw] == brightness_dig[dispketaw]){ // クロスフェードカウンタ最大値（輝度）到達
    disp_last[dispketaw] = disp[dispketaw];                 // 前回データに今回データをコピー
    disp_fadecount[dispketaw] = 0;                          // クロスフェードカウンタクリア
  }

// Data Output
#ifdef DELETE
  //SPI Data transfer
  portENTER_CRITICAL_ISR(&timerMux);    // 割り込み禁止
  vspiCommand3byte(dispdata);                               // 表示データ転送
  portEXIT_CRITICAL_ISR(&timerMux);     // 割り込み許可
#endif

  digitalWrite(SPISS_PIN, LOW) ;
  vfd_data_trans(uint8_t(dispdata & 0xFF));
  vfd_data_trans(uint8_t((dispdata >> 8) & 0xFF));
  vfd_data_trans(uint8_t((dispdata >> 16) & 0xFF));
  digitalWrite(SPISS_PIN, HIGH) ;                     // ラッチ信号を出す

  return;
}
