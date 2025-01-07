#ifndef vfd_web_h
#define vfd_web_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#ifdef ARDUINO_M5Stack_Core_ESP32
  #include <M5Stack.h>
#elif ARDUINO_M5Stick_C
  #include <M5StickCPlus.h>
#elif ARDUINO_M5Stack_ATOM
//  #include <M5Atom.h>
#endif

#include <M5Unified.h>

#define USE_LittleFS

#include <FS.h>
#ifdef USE_LittleFS
  #define SPIFFS LittleFS
  #include <LittleFS.h> 
#else
  #include <SPIFFS.h>
#endif 

#include <WebServer.h>
//#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h> // arduinoWebSocketsライブラリ

#define STA_DISCONNECTED  0     // 接続なし:使用しない
#define STA_CONNECTED     1     // 接続完了
#define STA_NO_SSID_AVAIL 2     // 指定されたSSID見つからなかった
#define STA_TIMEOUT       3     // タイムアウト


class WebsocketDataSend{
  public:
    WebsocketDataSend(void);
    void websocketSend(void);
    void websocketEventSend(struct eventQueData);

    void setReady(bool);            // WebSocket送信許可設定
    bool getReady(void);            // WebSocket送信許可取得

    bool dataReady(void);           // 送信データ準備OK

    bool basicDataSend;             // 基本データ送信フラグ
    struct tm timeInfo;
    uint16_t sensorData[8];

    bool wifiStaListSend;
    String wifiStaList;

    bool staSsidSend;               // StationMode 接続情報送信フラグ
    String staSsid;                 // StationMode SSID
    String staIpadr;                // StationMode IP Adress
    uint8_t staStatus;              // StationMode WiFi接続状態


  private:
    bool ready;                     // WebSocket送信許可

};
GLOBAL  WebsocketDataSend websocketDataSend;

void setWebhandle(void);                      // Web handle set
void webHandleClient(void);                   // webサーバ処理
void startWebserver(void);
//void websocketSend(struct tm ,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
void websocketSend(struct websocketData);

#undef GLOBAL
#endif
