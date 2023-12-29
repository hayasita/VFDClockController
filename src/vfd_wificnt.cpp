#include "vfd_wificnt.h"
#include "vfd_conf.h"
#include "vfd_rtc.h"
#include "vfd_driver.h"
#include "vfd_web.h"

uint8_t wifiConSts;                   // WiFi接続シーケンス
#define WiFiCONSTS_NOCON          0   // 接続なし

#define WiFiCONSTS_SNTPAUTOIDLE1  10  // SNTP要求接続試行中
#define WiFiCONSTS_SNTPAUTOIDLE2  11  // SNTP要求接続試行中
#define WiFiCONSTS_SNTPAUTOIDLE3  12  // SNTP要求接続試行中
#define WiFiCONSTS_SNTPAUTOCON1   13  // SNTP要求接続中
#define WiFiCONSTS_SNTPAUTOCON2   14  // SNTP要求接続中

#define WiFiCONSTS_MANIDLE1       20  // 手動接続試行中1
#define WiFiCONSTS_MANIDLE2       21  // 手動接続試行中2
#define WiFiCONSTS_MANIDLE3       22  // 手動接続試行中3
#define WiFiCONSTS_MANIDLE4       23  // 手動接続試行中4
#define WiFiCONSTS_MANIDLE5       24  // 手動接続試行中5
#define WiFiCONSTS_MANIDLE6       25  // 手動接続試行中6

#define WiFiCONSTS_MANCON         30  // 手動接続中
#define WiFiCONSTS_DISCON1        31  // 切断
#define WiFiCONSTS_DISCON2        32  // 切断

#define WiFiCONSTS_STARECON1      40  // STA再接続1
#define WiFiCONSTS_STARECON2      41  // STA再接続1
#define WiFiCONSTS_STARECON3      42  // STA再接続1

const char *apSsid = "IV-21Clock";
const char *apPassword = "";
const char* MDNS_NAME = apSsid;
bool wifiApStop;

char staSsid[33];               // WiFi Station mode SSID
char staSsidPassword[64];       // WiFi Station mode SSID Password

/* WiFI modeSet*/
void wifiModeSet(wifi_mode_t mode,uint8_t nextSqf)
{
  bool ret;

  ret = WiFi.mode(mode);
  if(ret == true){
    Serial.println("WiFi.mode 成功");
    wifiConSts = nextSqf;
  }
  else{
    Serial.println("WiFi.mode 失敗");
  }

  return;
}

/* WiFi STA接続 */
bool staConnect(char *ssid,char *pass,uint8_t nextSqf)
{
  bool ret;
/*
  if(strlen(ssid) == 0){
    ret = WiFi.begin("","");
  }
  else{
    ret = WiFi.begin("ssid","pass");
  }
*/  
  ret = WiFi.begin();

  if(ret == true){
    Serial.println("WiFi.begin 成功");
    wifiConSts = nextSqf;
  }
  else{
    Serial.println("WiFi.begin 失敗");
  }

  return ret;
}

// STA接続情報設定
void setStaStatus(uint8_t status)
{
  websocketDataSend.staSsidSend = ON;

  if(status == STA_CONNECTED){
    // SSID,IP Adress 設定
    websocketDataSend.staSsid = getWifiStaSsid();
    websocketDataSend.staIpadr = getWifiStaIpadr();
  }
  else{
    // SSID,IP Adress クリア
    websocketDataSend.staSsid = String("");
    websocketDataSend.staIpadr = String("");
  }
  websocketDataSend.staStatus = status;

  return;
}
/* WiFi STA 接続完了待ち */
void staConnectionWait(uint8_t nextSqf,uint8_t errSqf)
{
  static uint8_t staConCount = 0;
  bool connectionFale = 0;
  if(WiFi.status() == WL_NO_SSID_AVAIL){
    Serial.println("-- 接続失敗 SSIDが無い。終了する。 WL_NO_SSID_AVAIL --");
    setStaStatus(STA_NO_SSID_AVAIL);  // STA接続情報設定：指定されたSSID見つからなかった
    connectionFale = 1;
  }
  else if(WiFi.status() == WL_CONNECTED){
    if(wifiConSts == WiFiCONSTS_MANIDLE6){
      Serial.println("-- 接続完了 : 手動 AP & STA --");
      vfdevent.setEventlogLoop(EVENT_WiFi_MANCON_STACOMP);   // 手動接続・WiFi STA接続完了
    }
    else if(wifiConSts == WiFiCONSTS_SNTPAUTOCON1){
      Serial.println("-- 接続完了 : SNTP自動接続 --");
      vfdevent.setEventlogLoop(EVENT_WIFI_AUTOCON_COMP);  // WiFi自動接続完了
    }
    else if(wifiConSts == WiFiCONSTS_STARECON3){
      Serial.println("-- 接続完了 : STA再接続 --");
      vfdevent.setEventlogLoop(EVENT_WiFi_STA_RECON_COMP);  // STA再接続・接続完了
    }
    staConCount = 0;
    setStaStatus(STA_CONNECTED);  // STA接続情報設定：接続完了
    wifiConSts = nextSqf;         // 接続完了時シーケンスに移行
  }
  else{
    Serial.print("WiFi.status() == ");
    Serial.println(WiFi.status());
    if(staConCount < 40){
      Serial.print(".");
      staConCount++;
    }
    else{
      Serial.println("-- 接続失敗 タイムアウト。終了する。 --");
      setStaStatus(STA_TIMEOUT);  // STA接続情報設定:タイムアウト
      connectionFale = 1;
      vfdevent.setEventlogLoop(EVENT_WiFi_STAERR_TIMEOUT);  // STA接続エラー・タイムアウト
    }
  }
  /* STA設定不備による接続失敗処理 */
  if(connectionFale == 1){
    WiFi.disconnect(true);
    if(ntpAutoSetSqf == RTCAUTO_CONNECTION){
      Serial.println("自動接続完了");
      ntpAutoSetSqf = RTCAUTO_STANDBY;  // SSID無効で、自動接続は待機に遷移
    }
    staConCount = 0;
//    wifiConSts = WiFiCONSTS_NOCON;   // 接続終了
    wifiConSts = errSqf;    //接続失敗時シーケンスに移行
  }

  return;
}

/* STA切断 */
void wifiDisconnect(uint8_t nextSqf)
{
  if((WiFi.disconnect(true) == true) || (WiFi.status() == WL_NO_SHIELD)){    // STA終了
    Serial.println("STA_OFF成功");
    Serial.print("// wifiStatus : ");
    Serial.println(WiFi.status());
    vfdevent.setEventlogLoop(EVENT_WiFi_DISCON);    // WiFI切断
//    eventWifiDiscon = true;   // WiFI切断イベント要求
    wifiConSts = nextSqf;
  }
  else{
    Serial.print(",");
//    Serial.print(WiFi.status());
  }
  return;
}

/* 接続中断 */
void wifiConnectEscape(uint8_t nextSqf)
{
  // ボタン入力で中断する
//  if(M5.BtnA.wasReleased()){             // ボタン短押し
  if(confWiFiStartf){
    confWiFiStartf = false;
    Serial.println("- 強制中断 -");
    vfdevent.setEventlogLoop(EVENT_WiFi_MAN_DISCON);    // WiFI手動中断
    ntpAutoSetSqf = RTCAUTO_STANDBY;    // 自動接続待機
    wifiConSts = nextSqf;
  }

  return;
}

// STAモードSSID取得
String getWifiStaSsid(void){
  return(WiFi.SSID());
}
// STAモードIPアドレス取得
String getWifiStaIpadr(void){
  return((WiFi.localIP()).toString());
}
// APモードSSID取得
String getWifiApSsid(void){
  return(WiFi.softAPSSID());
}
// APモードIPアドレス取得
String getWifiApIpadr(void){
  return((WiFi.softAPIP()).toString());
}

// WiFI接続制御
bool wifiConnectMan(void)
{
  static unsigned long timetmp;
  static unsigned long sntpTimeoutChk;
  static wifi_mode_t mode;
  static uint8_t staConCount;
  bool ret;
  bool wifiOn = OFF;

  if(wifiConSts == WiFiCONSTS_NOCON){
  // 接続なし
    led.setLedMode(LEDMODE_WIFI_NO_CONNECT);    // LED消去

    if(vfdevent.eventLogSendSqf == EVENT_LOGSEND_WAIT){
      vfdevent.eventLogSendSqf = EVENT_LOGSEND_WAIT;  // イベントLOG送信終了
    }

    // STA再接続要求クリア
    wifiStaReconnect = 0;

    // websocket開始フラグクリア
    websocketConnect = false;

    if(ntpAutoSetSqf == RTCAUTO_CONNECTION){ // NTP接続要求
      wifiConSts = WiFiCONSTS_SNTPAUTOIDLE1;
      vfdevent.setEventlogLoop(EVENT_WIFI_AUTOCON_START);     // WiFi自動接続
    }
//    if(M5.BtnA.wasReleased()){
    if(confWiFiStartf){
    // WiFi手動接続・ボタン短押し
      confWiFiStartf = false;
      timetmp = millis() - 500;
      mode = WIFI_MODE_APSTA;
      wifiConSts = WiFiCONSTS_MANIDLE1;
    }
  }
  // -----
  else if(wifiConSts == WiFiCONSTS_SNTPAUTOIDLE1){
  // SNTP要求自動接続・WiFiモード設定
    led.setLedMode(LEDMODE_SNTP_STARTUP_CONNECT_START);  // WiFi手動接続開始~接続完了

    if(millis() - timetmp > 500){
      timetmp = millis();
      Serial.println("-- STA接続要求:WiFiCONSTS_SNTPAUTOIDLE1 --");
      wifiModeSet(mode,WiFiCONSTS_SNTPAUTOIDLE2);     // WiFI modeSet
    }
    wifiConnectEscape(WiFiCONSTS_DISCON2);  // ボタン入力で中断する
  }
  else if(wifiConSts == WiFiCONSTS_SNTPAUTOIDLE2){
  // SNTP要求自動接続・WiFi接続
    if(millis() - timetmp > 500){
      timetmp = millis();
      staConnect((char *)"",(char *)"",WiFiCONSTS_SNTPAUTOIDLE3);   // WiFi STA接続
    }
    wifiConnectEscape(WiFiCONSTS_DISCON2);  // ボタン入力で中断する
  }
  else if(wifiConSts == WiFiCONSTS_SNTPAUTOIDLE3){
  // WiFi STA 接続完了待ち
    if(millis() - timetmp > 500){
      //Serial.println("-- SNTP要求接続試行中 --");
      timetmp = millis();   // timmer set
      staConnectionWait(WiFiCONSTS_SNTPAUTOCON1,WiFiCONSTS_NOCON);   // WiFi STA 接続完了待ち
    }
    wifiConnectEscape(WiFiCONSTS_DISCON2);  // ボタン入力で中断する
  }
  // -----
  else if(wifiConSts == WiFiCONSTS_SNTPAUTOCON1){
  //  WiFi自動接続完了・SNTP初期化

  // SNTP要求接続中
    led.setLedMode(LEDMODE_SNTP_STARTUP_CONNECT);  // WiFi手動接続開始~接続完了

    Serial.println("-- SNTP要求接続完了 --");
    // SNTP初期化
    sntpCont.init();
    Serial.printf("WiFiGotIP: SNTP sync mode = %d (0:IMMED 1:SMOOTH)\n", sntp_get_sync_mode());
    Serial.printf("WiFiGotIP: SNTP sync status = %d (0:IMMED 1:SMOOTH)\n", sntp_get_sync_status());

    timetmp = millis() - 500;
    sntpTimeoutChk = millis();    // SNTP Timeout Counter init
    wifiConSts = WiFiCONSTS_SNTPAUTOCON2;
  }
  else if(wifiConSts == WiFiCONSTS_SNTPAUTOCON2){
  // SNTP処理完了まち
  // SNTP処理完了したらWiFiCONSTS_NOCONに戻る
//    sntp_sync_status_t sntp_sync_status = sntp_get_sync_status();
//    if ((sntp_sync_status == SNTP_SYNC_STATUS_COMPLETED) ||(sntpCompleted == true)){
//    if (sntp_sync_status == SNTP_SYNC_STATUS_COMPLETED){
    if (sntpCompleted == true){   // SNTP同期完了フラグ設定？
      Serial.println("-- SNTP処理完了:WiFiCONSTS_SNTPAUTOCON2 --");
      sntpCompleted = false;      // SNTP同期完了フラグリセット
//      vfdevent.setEventlogLoop(EVENT_WIFI_AUTOCON_SNTP_COMP);   // SNTP処理完了
      timetmp = millis() - 500;
      ntpAutoSetSqf = RTCAUTO_DISCONNECTION;
      wifiConSts = WiFiCONSTS_DISCON2;
    }
    else if((millis() - sntpTimeoutChk > (2*3600*1000)) ){  // 2時間で中止する 
      Serial.println("-- SNTP TimeOut Abote.");
      vfdevent.setEventlogLoop(EVENT_WIFI_AUTOCON_SNTP_TIMEOUT);  // SNTP処理タイムアウト
      timetmp = millis() - 500;
      ntpAutoSetSqf = RTCAUTO_DISCONNECTION;
//      ntpAutoSetSqf = RTCAUTO_STANDBY;
      wifiConSts = WiFiCONSTS_DISCON2;
    }
    else if(millis() - timetmp > 500){
      Serial.print(",");
      timetmp = millis();
    }
    wifiConnectEscape(WiFiCONSTS_DISCON2);  // ボタン入力で中断する
  }
  // -- //
  else if(wifiConSts == WiFiCONSTS_MANIDLE1){
  // 手動接続・WiFi APモード設定
    led.setLedMode(LEDMODE_WIFI_MANUAL_CONNECT_START);  // WiFi手動接続開始~接続完了

    if(millis() - timetmp > 500){
      timetmp = millis();
      Serial.println("-- 手動接続試行中 --");
      wifiModeSet(mode,WiFiCONSTS_MANIDLE2);
    }
    wifiConnectEscape(WiFiCONSTS_DISCON1);  // ボタン入力で中断する
  }
  else if(wifiConSts == WiFiCONSTS_MANIDLE2){
  // 手動接続・WiFi APモード起動
    if(millis() - timetmp > 500){
      Serial.println("-- AP起動 --");
      ret = WiFi.softAP(apSsid, apPassword);
      if(ret == true){
        Serial.println("WiFi.softAP 成功");
        vfdevent.setEventlogLoop(EVENT_WIFI_MANCON_APSTART);      // 手動接続・WiFi APモード起動
        wifiConSts = WiFiCONSTS_MANIDLE3;
      }
      else{
        Serial.println("WiFi.softAP 失敗");
        vfdevent.setEventlogLoop(EVENT_WIFI_MANCON_APSTARTFAIL);  // 手動接続・WiFi APモード起動失敗
      }
      timetmp = millis();   // timmer set
    }
    wifiConnectEscape(WiFiCONSTS_DISCON1);  // ボタン入力で中断する
  }
  else if(wifiConSts == WiFiCONSTS_MANIDLE3){
  // 手動接続・WiFi AP起動・Server起動
    if(millis() - timetmp > 100){
      // Soft AP 接続成功
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());
      startWebserver();     //WebServer Start
      vfdevent.setEventlogLoop(EVENT_WIFI_MANCON_APCOMP);   // 手動接続・WiFi APモード起動完了
      timetmp = millis();   // timmer set
      wifiConSts = WiFiCONSTS_MANIDLE4;
    }
  }
  else if(wifiConSts == WiFiCONSTS_MANIDLE4){
  // mDNS起動
    if(millis() - timetmp > 100){
      // mDNSに名前登録 
      timetmp = millis();   // timmer set
      if (MDNS.begin(MDNS_NAME)) {
        Serial.println("MDNS responder started");
        Serial.print("MDNS HOST Name: ");
        Serial.println(MDNS_NAME);
        if(mode == WIFI_MODE_APSTA){               // SSID設定あり
          Serial.println("-- AP接続完了・STA接続要求 --");
          vfdevent.setEventlogLoop(EVENT_WIFI_MANCON_STASTART);   // // 手動接続・WiFi STA起動
          wifiConSts = WiFiCONSTS_MANIDLE5;
        }
        else{
          Serial.println("-- 接続完了 : AP --");
          wifiConSts = WiFiCONSTS_MANCON;
        }
      }
    }
    wifiConnectEscape(WiFiCONSTS_DISCON1);  // ボタン入力で中断する
  }
  else if(wifiConSts == WiFiCONSTS_MANIDLE5){
  // 手動接続・WiFi STA起動
    if(millis() - timetmp > 500){
      timetmp = millis();

      if(mode == WIFI_MODE_APSTA){               // SSID設定あり
        Serial.println("-- STA接続要求 --");
        staConnect((char *)"",(char *)"",WiFiCONSTS_MANIDLE6);
      }
      else{
        // SSID設定無い場合は起動完了
        Serial.println("-- 接続完了 : AP --");
        wifiConSts = WiFiCONSTS_MANCON;
      }
    }
    wifiConnectEscape(WiFiCONSTS_DISCON1);  // ボタン入力で中断する
  }
  else if(wifiConSts == WiFiCONSTS_MANIDLE6){
  // 手動接続・WiFi STA接続待ち
    if(millis() - timetmp > 500){
      timetmp = millis();   // timmer set
      staConnectionWait(WiFiCONSTS_MANCON,WiFiCONSTS_MANCON);   // WiFi STA 接続完了待ち
    }
    wifiConnectEscape(WiFiCONSTS_DISCON1);  // ボタン入力で中断する
  }
  else if(wifiConSts == WiFiCONSTS_MANCON){
  // 手動接続完了
    led.setLedMode(LEDMODE_WIFI_MANUAL_CONNECT);  // WiFi手動接続開始~接続完了

    wifiOn = ON;  // WiFi接続ON

    // 切断処理開始
//    if(M5.BtnA.wasReleased()){             // ボタン短押し
    if(confWiFiStartf){
      confWiFiStartf = false;
      timetmp = millis() - 500;
      staConCount = 0;
      wifiApStop = OFF;
      Serial.println("-- AP終了開始 --");
      wifiConSts = WiFiCONSTS_DISCON1;
    }

    // STA再接続
    if(wifiStaReconnect == 1){
      wifiStaReconnect = 0;
      Serial.println("-- STA再接続要求 --");
      Serial.print("WiFi.status():");
      Serial.println(WiFi.status());
      timetmp = millis() - 500;
      wifiConSts = WiFiCONSTS_STARECON1;  // 自動再接続要求
    }
  }
  else if(wifiConSts == WiFiCONSTS_STARECON1){
  // STA再接続・切断
    wifiOn = ON;  // WiFi接続ON
    static uint8_t disconCount = 0;
    if(millis() - timetmp > 500){
      // WiFi STA切断・設定情報を削除
      if( (WiFi.disconnect(true,true) == true) || (disconCount == 20)
       || (WiFi.status() == WL_DISCONNECTED) || (WiFi.status() == WL_NO_SHIELD) ){    // STA終了
        Serial.println("STA_OFF成功");
        vfdevent.setEventlogLoop(EVENT_WiFi_STA_RECON_DISCON);   // STA再接続・切断
        timetmp = millis() - 500;
        strcpy(staSsid,confDat.getSsid());
        strcpy(staSsidPassword,confDat.getSsidPass());
        wifiConSts = WiFiCONSTS_STARECON2;
      }
      else{
        Serial.println(".");
        timetmp = millis();
        disconCount++;
      }
    }
  }
  else if(wifiConSts == WiFiCONSTS_STARECON2){
  // STA再接続・接続
    wifiOn = ON;  // WiFi接続ON
    led.setLedMode(LEDMODE_WIFI_MANUAL_CONNECT_START);  // WiFi手動接続開始~接続完了
    Serial.println("-- STA再接続・接続 --");

    if(strcmp(confDat.getSsid(),"0") == 0){
      WiFi.begin("0","0");
      mode = WIFI_MODE_AP;
      vfdevent.setEventlogLoop(EVENT_WiFi_STA_RECON_SSID_DELETE);   // STA再接続・接続
      Serial.println("設定削除");
    }
    else{
      WiFi.begin(staSsid,staSsidPassword);
      mode = WIFI_MODE_APSTA;
      vfdevent.setEventlogLoop(EVENT_WiFi_STA_RECON_CON_START);   // STA再接続・接続
      Serial.println("新規設定で再接続");
      Serial.print("getSsid : ");
      Serial.println(staSsid);
      Serial.print("getSsidPass : ");
      Serial.println(staSsidPassword);
    }
    timetmp = millis();   // timmer set
    strcpy(staSsid,"");             // Webから送信されたSSIDをクリア
    strcpy(staSsidPassword,"");     // Webから送信されたSSID Passwordをクリア
    confDat.clearStaData();         // Webから送信されたSSID,Passwordをクリア
    wifiConSts = WiFiCONSTS_STARECON3;
  }
  else if(wifiConSts == WiFiCONSTS_STARECON3){
  // STA再接続・接続完了待ち
    wifiOn = ON;  // WiFi接続ON
    if(millis() - timetmp > 500){
      timetmp = millis();   // timmer set
      staConnectionWait(WiFiCONSTS_MANCON,WiFiCONSTS_MANCON);   // WiFi STA 接続完了待ち
    }
    wifiConnectEscape(WiFiCONSTS_DISCON1);  // ボタン入力で中断する
  }
  // -- //
  else if(wifiConSts == WiFiCONSTS_DISCON1){
  // WiFi AP切断
    led.setLedMode(LEDMODE_WIFI_MANUAL_DISDISCONNECT);  // WiFi手動接続開始~接続完了

  // 切断
    if(millis() - timetmp > 500){
//      Serial.println("-- 切断 --");
      if((WiFi.softAPdisconnect(true) == true) || (wifiApStop == ON) || (staConCount > 120)){     // AP終了
        Serial.println("AP_OFF成功");
        Serial.println("staConCount : ");
        Serial.println(staConCount);

        if(mode == WIFI_MODE_APSTA){
          timetmp = millis() - 500;
          Serial.println("-- STA終了開始 --");
          wifiConSts = WiFiCONSTS_DISCON2;
        }
        else{
          wifiConSts = WiFiCONSTS_NOCON;  // STA無いので切断完了
        }
      }
      else{
//        Serial.println("AP_OFF失敗");
        staConCount ++;
        Serial.print(".");
        timetmp = millis();
//        Serial.print("** wifiStatus : ");
//        Serial.println(WiFi.status());
      }
    }
  }
  else if(wifiConSts == WiFiCONSTS_DISCON2){
  // WiFI STA切断
    if(millis() - timetmp > 500){
      timetmp = millis();
      wifiDisconnect(WiFiCONSTS_NOCON);
    }
  }

  return wifiOn;
}

//unsigned long sntpAutoConSqTime;  // SNTP取得自動接続シーケンスタイマ
// sntp自動接続要求
bool ntpAutoSetreq(void)
{
  bool ret = true;

  if(ntpAutoSetSqf == RTCAUTO_STANDBY){       // 接続待機

    if( (confDat.getNtpset() == 1)            // ntp使用選択有効
    && (confDat.getStaAutoConnectSel() == 1)  // STA自動接続有効
    ){  // NTP使用時は接続要求設定する。
//      sntpAutoConSqTime = millis();         // SNTP取得自動接続シーケンスタイマ初期化
      ntpAutoSetSqf = RTCAUTO_CONNECTION; // 接続
      vfdevent.setEventlogDeviceCtrl(EVENT_SNTP_STARTCON_REQ);   // SNTP取得　起動時接続動作要求
      Serial.println("++ SNTP取得　起動時接続動作要求");
    }
  }
  else{
    ret = false;
  }

  return ret;
}


// Set WiFi Call Back.
void setWiFiCallBack(void)
{
//  WiFi.onEvent(WiFiStationConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED/*SYSTEM_EVENT_STA_CONNECTED*/);
  WiFi.onEvent(WiFiStationStartCollBack, ARDUINO_EVENT_WIFI_STA_START/*SYSTEM_EVENT_STA_START*/);   // CallBack ESP32 station start
  WiFi.onEvent(WiFiStationStopCollBack, ARDUINO_EVENT_WIFI_STA_STOP/*SYSTEM_EVENT_STA_STOP*/);     // CallBack ESP32 station stop
  WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP/*SYSTEM_EVENT_STA_GOT_IP*/);                 // IPアドレス取得時イベントコールバック設定
//  WiFi.onEvent(WiFiStationDisconnectedCollBack, ARDUINO_EVENT_WIFI_STA_CONNECTED /*SYSTEM_EVENT_STA_DISCONNECTED*/);
  WiFi.onEvent(WiFiAPStartCollBack, ARDUINO_EVENT_WIFI_AP_START/*SYSTEM_EVENT_AP_START*/);         // CallBack ESP32 soft-AP start
  WiFi.onEvent(WiFiAPStopCollBack, ARDUINO_EVENT_WIFI_AP_STOP/*SYSTEM_EVENT_AP_STOP*/);           // CallBack ESP32 soft-AP stop
  WiFi.onEvent(WiFiSoftAPConnected, ARDUINO_EVENT_WIFI_AP_STACONNECTED/*SYSTEM_EVENT_AP_STACONNECTED*/);  // CallBack a station connected to ESP32 soft-AP

  return;
}

// CallBack ESP32 station start
void WiFiStationStartCollBack(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("\n== CallBack ESP32 station start.");
  return;
}
// CallBack ESP32 station stop
void WiFiStationStopCollBack(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("\n== CallBack ESP32 station stop.");
  return;
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  return;
}

// IPアドレス取得時イベントコールバック
// CallBack ESP32 station got IP from connected AP
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("\n== WiFiGotIP!!");
  return;
}
// CallBack ESP32 station disconnected from AP 
void WiFiStationDisconnectedCollBack(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("\n== CallBack ESP32 station disconnected from AP.");
  return;
}
// CallBack ESP32 soft-AP start 
void WiFiAPStartCollBack(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("\n== CallBack ESP32 soft-AP start.");
  return;
}
// CallBack ESP32 soft-AP stop
void WiFiAPStopCollBack(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("\n== CallBack ESP32 soft-AP stop.");
  wifiApStop = ON;

  return;
}
// CallBack a station connected to ESP32 soft-AP
void WiFiSoftAPConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("\n== CallBack a station connected to ESP32 soft-AP.");
  return;
}


String getwifiStaList(void)    // WiFi Stationリスト取得
{
  #define SSIDLIMIT 30
  String ssid_rssi_str[SSIDLIMIT];
  String ssid_str[SSIDLIMIT];
//  String str = "{\n\"stationList\":[\n";
  String str = "\"stationList\":[\n";

  Serial.println("wifi scan start");

  // WiFi.scanNetworks will return the number of networks found
  uint8_t ssid_num = WiFi.scanNetworks();
  Serial.println("scan done\r\n");
  
  if (ssid_num == 0) {
    Serial.println("no networks found");
  } else {
    Serial.printf("%d networks found\r\n\r\n", ssid_num);
    if (ssid_num > SSIDLIMIT) ssid_num = SSIDLIMIT;
    for (int i = 0; i < ssid_num; ++i) {
      ssid_str[i] = WiFi.SSID(i);
      String wifi_auth_open = ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");

      ssid_rssi_str[i] = ssid_str[i] + " (" + WiFi.RSSI(i) + "dBm)" + wifi_auth_open;

      if(i != 0){
        str = str + ",\n";
      }
      str = str + "{\"ID\":\"" + ssid_str[i] + "\",\"TITLE\":\"" + ssid_rssi_str[i] + "\"}";
//      ssid_rssi_str[i] = ssid_str[i] + " (" + WiFi.RSSI(i) + "dBm)" + wifi_auth_open;
    //  ssid_rssi_str[i] = ssid_str[i] + wifi_auth_open;
      Serial.printf("%d: %s\r\n", i, ssid_rssi_str[i].c_str());
    //  delay(10);
    }
//    str = str + "\n]\n}";
    str = str + "\n]";
  }

  Serial.print("\n");
  Serial.println(str);

  return str;
}
