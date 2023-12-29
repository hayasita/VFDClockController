#ifndef vfd_wificnt_h
#define vfd_wificnt_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#ifdef ARDUINO_M5Stack_Core_ESP32
//  #include <M5Stack.h>
#elif ARDUINO_M5Stick_C
//  #include <M5StickCPlus.h>
#elif ARDUINO_M5Stack_ATOM
//  #include <M5Atom.h>
#endif

#include <M5Unified.h>

#include <WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESPmDNS.h>
#include <sntp.h>

GLOBAL uint8_t ntpAutoSetSqf; // NTP AutoConnection SQF
#define RTCAUTO_STANDBY       0   // 待機
#define RTCAUTO_CONNECTION    1   // 接続
#define RTCAUTO_DISCONNECTION 2   // 切断
bool ntpAutoSetreq(void);         // sntp自動接続要求

GLOBAL uint8_t wifiStaReconnect;  // STAmode SSID再設定フラグ
GLOBAL uint8_t getwifiStaListreq; // WiFiStationList取得要求
GLOBAL bool websocketConnect;     // websocket開始フラグ

bool wifiConnectMan();
String getWifiStaSsid(void);    // STAモードSSID取得
String getWifiStaIpadr(void);   // STAモードIPアドレス取得
String getWifiApSsid(void);     // APモードSSID取得
String getWifiApIpadr(void);    // APモードIPアドレス取得
String getwifiStaList(void);    // WiFi Stationリスト取得
GLOBAL uint8_t getwifiListsqf;

GLOBAL void setWiFiCallBack(void);  // Set WiFi Call Back.

void WiFiStationStartCollBack(WiFiEvent_t event, WiFiEventInfo_t info);   // CallBack ESP32 station start
void WiFiStationStopCollBack(WiFiEvent_t event, WiFiEventInfo_t info);    // CallBack ESP32 station stop
void WiFiStationConnected(WiFiEvent_t, WiFiEventInfo_t);  // WiFi接続時コールバック
void WiFiGotIP(WiFiEvent_t, WiFiEventInfo_t);             // WiFi IP Adress取得コールバック
void WiFiManualConnectCollBack(WiFiEvent_t , WiFiEventInfo_t );
void WiFiStationDisconnectedCollBack(WiFiEvent_t event, WiFiEventInfo_t info);  // // CallBack ESP32 station disconnected from AP 
void WiFiAPStartCollBack(WiFiEvent_t event, WiFiEventInfo_t info);        // CallBack ESP32 soft-AP start 
void WiFiAPStopCollBack(WiFiEvent_t event, WiFiEventInfo_t info);         // CallBack ESP32 soft-AP stop
void WiFiSoftAPConnected(WiFiEvent_t event, WiFiEventInfo_t info);        // CallBack a station connected to ESP32 soft-AP

#undef GLOBAL
#endif
