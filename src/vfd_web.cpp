#include "vfd_web.h"
#include "vfd_wificnt.h"
#include "vfd_conf.h"
//#include "vfd_driver.h"
#include "vfd_rtc.h"
#include "jsdata.h"
#include <ArduinoJson.h>

// Webサーバ
#define HTTP_PORT 80
WebServer server(HTTP_PORT);

// Websocketサーバ
WebSocketsServer webSocket = WebSocketsServer(81); // 81番ポート
uint8_t socketid;
void webSocketEvent(uint8_t , WStype_t , uint8_t * , size_t );  // websocketコールバック関数

void handleRootGet(void);
void handleRootPost(void);
void handleSettingjs(void);
void handleNotFound();
String getContentType(String);
bool handleFileRead(String);

// webサーバ処理
void webHandleClient(void)
{
  server.handleClient();
  webSocket.loop();
}

// webサーバ初期化  
void startWebserver(void)
{
  server.begin();
  Serial.println("HTTP server started");
  // WebSocketサーバー開始
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);    // websocketコールバック関数設定
  Serial.println("WebSocket server started");

  return;
}


// websocket 受信データ設定処理
void websocketDataSet(char * payload)
{
  DynamicJsonDocument doc(1024);
  String jsonStr;
  jsonStr = payload;

  settingjsFile.setJsonData(jsonStr,&confDat,1);
#ifdef DELETE  
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) {
    // エラーの場合
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else{
    // changeTab
    JsonVariant changeTab = doc["tabName"];
    if (!changeTab.isNull()) {
      const char* s = changeTab.as<const char*>();    //testtest
      Serial.println(s);
      confDat.setTabName(s);
    }
  }
#endif

  return;
}

// websocketコールバック関数
//    websocket データ受信処理
//    num: クライアント番号
//    type: イベント種別
//    payload, length: 受信データとそのサイズ
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  switch(type) {
    case WStype_DISCONNECTED:
    //  切断時の処理;
      Serial.println("WStype_DISCONNECTED :");
      websocketDataSend.setReady(OFF);   // WebSocket送信不許可設定
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num); // クライアントのIPアドレスを取得
        Serial.print("WStype_CONNECTED :");
        Serial.println(ip);
//        Serial.println(num);
        socketid = num;
      // 接続時の処理
#ifdef OLD_EVENTLOG
        vfdevent.resetEventSendPt();      // イベントデータ送信ポインタクリア
#endif
        websocketDataSend.setReady(ON);   // WebSocket送信許可設定
      }
      break;
    case WStype_TEXT:
    //  テキストデータ受信時の処理;
//      Serial.print("WStype_TEXT :");
//      Serial.printf("[%u] get Text: %s: len: %d\n", num, payload,length);
      char temp[3000];
      snprintf(temp, 3000, "%s", payload);
      websocketDataSet(temp);
      break;
    case WStype_BIN:
    //  バイナリデータ受信時の処理;
      Serial.println("WStype_BIN :");
      break;
    case WStype_ERROR:
    //  エラー時の処理;
      Serial.println("WStype_ERROR :");
      break;
  }
  return;
}

// Web handle set
void setWebhandle(void)
{
  // アクセスされた際に行う関数を登録する
  server.on("/", HTTP_GET, handleRootGet);
  server.on("/", HTTP_POST, handleRootPost);
  server.on("/setting.js", handleSettingjs);

  // 登録されてないパスにアクセスがあった場合、ファイルアクセスの応答関数を設定
  SPIFFS.begin();  // SPIFFS Initial setting
  server.onNotFound(handleNotFound);

  return;
}

// 転送するファイルの処理
void handleNotFound()
{
  if (! handleFileRead(server.uri())) {
    //  ファイルが見つかりません
    Serial.println("404 not found");
    server.send(404, "text/plain", "File not found in Dongbeino...");
  }
}

//  MIMEタイプを推定
String getContentType(String filename)
{
  if(filename.endsWith(".html") || filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else return "text/plain";
}

//  SPIFSS のファイルをクライアントに転送する
bool handleFileRead(String path)
{
  Serial.println("handleFileRead: trying to read " + path);
  // パス指定されたファイルがあればクライアントに送信する
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  Serial.println("contentType: " + contentType);
  if (SPIFFS.exists(path)) {
    Serial.println("handleFileRead: sending " + path);
    File file = SPIFFS.open(path, "r");
//    server.sendHeader("Cache-Control", "public, max-age=86400");  // cache設定
    server.streamFile(file, contentType);
    file.close();
    Serial.println("handleFileRead: sent " + path);
    return true;
  }
  else {
    Serial.println("handleFileRead: 404 not found");
    server.send (404, "text/plain", "ESP: 404 not found");
    return false;
  }
}

//
// /setting.js 作成処理
//
void handleSettingjs(void){
  String html_tmp = "";

  html_tmp = makeSettingjs();

  server.send(200, "application/javascript", html_tmp);

}

/*
  POST データ受信処理
*/
void handleRootPost() {
//  DynamicJsonDocument doc(512);
  String jsonStr;

  Serial.println("HTTP_POST");
  jsonStr = server.arg("plain"); // server.arg("plain")でリクエストボディが取れる。targetに格納
  Serial.println(jsonStr);

  settingjsFile.setJsonData(jsonStr,&confDat,1);
#ifdef DELETE
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) {
    // エラーの場合
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else{
    Serial.println(doc.size());

  }
#endif
  server.send(200, "text/plain", jsonStr); // 値をクライアントに返す
  Serial.println(jsonStr);
}

// HTTP_GET / 処理
void handleRootGet() {
  uint8_t count;
  Serial.println("HTTP_GET");
  Serial.println(server.args());
  Serial.println(server.uri());
  Serial.println(server.method());

  if(server.args() > 0){
    Serial.println(server.argName(0));
    Serial.println(server.arg(0));
    for(count = 0;count <= server.args();count++){
//      Serial.print(server.argName(count));
//      Serial.print(" : ");
//      Serial.println(server.arg(count));
    }
  }
  handleFileRead("/index.html");

  return;
}

//const char SENSOR_JSON[] PROGMEM = R"=====({"sensor":{"val1":%d,"val2":%d,"val3":%d,"val4":%d,"val5":%d,"val6":%d,"val7":%d,"val8":%d}})=====";
//const char SENSOR_JSON[] PROGMEM = R"=====({"sensor":[{ "name": "name1", "data": %d },{ "name": "name2", "data": %d },{ "name": "name3", "data": %d },{ "name": "name4", "data": %d },{ "name": "name5", "data": %d },{ "name": "name6", "data": %d },{ "name": "name7", "data": %d },{ "name": "name8", "data": %d }]})=====";
const char SENSOR_JSON[] PROGMEM = R"=====({"time":{"year":%d,"month":%d,"date":%d,"hour":%d,"min":%d,"sec":%d},"sensor":[{ "name": "name1", "data": %d },{ "name": "name2", "data": %d },{ "name": "name3", "data": %d },{ "name": "name4", "data": %d },{ "name": "name5", "data": %d },{ "name": "name6", "data": %d },{ "name": "name7", "data": %d },{ "name": "name8", "data": %d }])=====";
const char NTPLASTUPDATE_JSON[] PROGMEM = R"=====(,"lastUpdate":{"Year" : %d,"Month" : %d,"Day" : %d,"Hour" : %d,"Min" : %d})=====";
const char STASSID_JSON[] PROGMEM = R"=====(, "staSsid":"%s", "staIpadr":"%s", "staStatus":%d)=====";
const char EVENTLOG_JSON[] PROGMEM = R"=====({"year":%u, "month":%d, "day":%d, "hour":%d, "min":%d, "sec":%d, "event":%d,"data":[%u,%u,%u,%u]})=====";

WebsocketDataSend::WebsocketDataSend(void)
{
  ready = OFF;
  wifiStaListSend = OFF;
//  lastUpdateSend = OFF;
  staSsidSend = OFF;        // StationMode 接続情報送信フラグ 初期化

  return;
}
void WebsocketDataSend::setReady(bool date)
{
  ready = date;
  return;
}
bool WebsocketDataSend::getReady(void)
{
  return ready;
}
// 送信データ準備OK
bool WebsocketDataSend::dataReady(void)
{
  bool ret = false;
  if(basicDataSend == ON){
    ret = true;
  }
  return ret;
}

void WebsocketDataSend::websocketSend(void)
{
//  Serial.println("websocketSend");

  char payload[1000];
  char payload1[200];
  String sendData;

  snprintf_P(payload, sizeof(payload), SENSOR_JSON, 
  ((uint16_t)timeInfo.tm_year+1900), timeInfo.tm_mon+1, timeInfo.tm_mday,
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec,
  sensorData[0], sensorData[1], sensorData[2], sensorData[3],
  sensorData[4], sensorData[5], sensorData[6], sensorData[7]);

  sendData = payload;
  basicDataSend = OFF;

  if(wifiStaListSend == ON){
    wifiStaListSend = OFF;
    Serial.println(wifiStaList);
//    strcat(payload,wifiStaList);
    const char *a = ",";
    sendData = sendData + a;
    sendData = sendData + wifiStaList; // 結合
  }
  // StationMode SSID,IP Adress送信
  if(staSsidSend == ON){
    staSsidSend = OFF;
    snprintf_P(payload1, sizeof(payload1), STASSID_JSON,
    staSsid.c_str(), staIpadr.c_str(), staStatus);
    sendData = sendData + payload1;
    Serial.println("staSsidSend : ");
    Serial.println(payload1);
  }

#ifdef OLD_EVENTLOG
  // EventLog送信
  EVENT_LOG elog;
//  Serial.println("-- Eventlog --");
//  Serial.println(vfdevent.getEventNum());
  if(vfdevent.withTransEventData()){    // イベントログ送信データあり
    vfdevent.getEventlog(&elog);        // イベントログ取得

    sendData = sendData + (String)", \"eventLog\":[";
    snprintf_P(payload1, sizeof(payload1), EVENTLOG_JSON,
    elog.year,elog.month,elog.day,elog.hour,elog.min,elog.sec,
    elog.event,0,0,0,0);
    sendData = sendData + payload1;
/*
    sendData = sendData + (String)",";

    snprintf_P(payload1, sizeof(payload1), EVENTLOG_JSON,
    websocketDataSend.evYear,websocketDataSend.evMonth,websocketDataSend.evDay,
    websocketDataSend.evHour,websocketDataSend.evMin,websocketDataSend.evSec,
    websocketDataSend.evEvent,0,0,0,0);
    sendData = sendData + payload1;
*/
    sendData = sendData + (String)"]";
    Serial.println("eventLogSend : ");
    Serial.println(payload1);
  }
#endif
  const char *a = "}";
  sendData = sendData + a;
//  webSocket.broadcastTXT(payload, strlen(payload)); // WebSocketでデータ送信(全端末へブロードキャスト)
//  webSocket.sendTXT(socketid, payload);             // socketidへpayloadを送信
  const char *c = sendData.c_str();
  webSocket.sendTXT(socketid, c);             // socketidへpayloadを送信

//  Serial.println(c);

  return;
}

// イベントLOG Websocket送信処理
void WebsocketDataSend::websocketEventSend(struct eventQueData eventQueRcvDat)
{
//  char payload[1000];
  char payload1[200];
  String sendData;

//  Serial.println("-- websocketEventSend --");

  sendData = sendData + (String)"{\"eventLog\":[";
  for(uint8_t i=0;i<eventQueRcvDat.dataSize;i++){
    if(i != 0){
      sendData = sendData + (String)",";
    }
    snprintf_P(payload1, sizeof(payload1), EVENTLOG_JSON,
      eventQueRcvDat.eventData[i].year,eventQueRcvDat.eventData[i].month,
      eventQueRcvDat.eventData[i].day,eventQueRcvDat.eventData[i].hour,
      eventQueRcvDat.eventData[i].min,eventQueRcvDat.eventData[i].sec,
      eventQueRcvDat.eventData[i].event,
      eventQueRcvDat.eventData[i].data[0],eventQueRcvDat.eventData[i].data[1],
      eventQueRcvDat.eventData[i].data[2],eventQueRcvDat.eventData[i].data[3]
    );
    sendData = sendData + (String)payload1;
  }
  sendData = sendData + (String)"]}";

//  Serial.println(((String)payload1).length());

  const char *c = sendData.c_str();
//  Serial.println(c);
  webSocket.sendTXT(socketid, c);             // socketidへpayloadを送信

  return;
}
