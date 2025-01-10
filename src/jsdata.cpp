
#include <vector>

#include "jsdata.h"
#include "vfd_conf.h"
#include "vfd_rtc.h"
#include "vfd_wificnt.h"

#include "filesys.h"

//#include <ArduinoJson.h>

// ./setting.js　作成
String makeSettingjs(void){
  String html_tmp = "";
  String stringTmp;
  char temp[3000];
  uint16_t tmp0;
  uint8_t tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;

  snprintf(temp, 3000, "var _initial_setting_ = \'{\\\n");
  html_tmp = html_tmp + String(temp);
// DashBord

// ネットワーク情報は接続状態によるので、値のチェックを行う。
// StationMode SSID
  stringTmp = getWifiStaSsid();
  if(stringTmp.length() != 0){
    html_tmp = html_tmp + (String)"\"stamodeSSID\" : \"" + stringTmp + (String)"\",\\\n";
  }
  else{
    html_tmp = html_tmp + (String)"\"stamodeSSID\" : \"\",\\\n";
  }
// StationMode IP Adress
  stringTmp = getWifiStaIpadr();
  if(stringTmp.length() != 0){    // todo. 0.0.0.0を検出して分岐するように修正必要。
    html_tmp = html_tmp + (String)"\"stamodeIP\" : \"" + stringTmp + (String)"\",\\\n";
  }
  else{
    html_tmp = html_tmp + (String)"\"stamodeIP\" : \"\",\\\n";
  }
// APMode SSID
  stringTmp = getWifiApSsid();
  if(stringTmp.length() != 0){
    html_tmp = html_tmp + (String)"\"atmodeSSID\" : \"" + stringTmp + (String)"\",\\\n";
  }
  else{
    html_tmp = html_tmp + (String)"\"atmodeSSID\" : \"\",\\\n";
  }
// APMode IP Adress
  stringTmp = getWifiApIpadr();
  if(stringTmp.length() != 0){
    html_tmp = html_tmp + (String)"\"atmodeIP\" : \"" + stringTmp + (String)"\",\\\n";
  }
  else{
    html_tmp = html_tmp + (String)"\"atmodeIP\" : \"\",\\\n";
  }

// 地域設定
  snprintf(temp, 3000,"\"localesId\" : %d,\\\n",confDat.getLocalesId());
  html_tmp = html_tmp + String(temp);

// Time settings data
//  ntpSet
  snprintf(temp, 3000,"\"ntpSet\" : %d,\\\n",confDat.getNtpset());
  html_tmp = html_tmp + String(temp);

// タイムゾーン
  html_tmp = html_tmp + (String)"\"timeZoneAreaId\" : \"" + confDat.getTimeZoneAreaId() + (String)"\",\\\n";
  html_tmp = html_tmp + (String)"\"timeZoneId\" : \"" + confDat.getTimeZoneId() + (String)"\",\\\n";
/*
// UTCとの時間差
  confDat.getDiffUtc(&tmp1,&tmp2);
  html_tmp = html_tmp + (String)"\"diffUtcHour\" : \"" + tmp1 + (String)"\",\\\n";
  html_tmp = html_tmp + (String)"\"diffUtcMin\" : \"" + tmp2 + (String)"\",\\\n";
*/
// 自動更新時刻
  confDat.getAutoUpdatetime(&tmp1,&tmp2);
  html_tmp = html_tmp + (String)"\"autoUpdateHour\" : \"" + tmp1 + (String)"\",\\\n";
  html_tmp = html_tmp + (String)"\"autoUpdateMin\" : \"" + tmp2 + (String)"\",\\\n";

// 最終更新時刻
/*
  confDat.getLastUpdate(&tmp0,&tmp2,&tmp3,&tmp4,&tmp5,&tmp6);
  html_tmp = html_tmp + (String)"\"lastUpdateYear\" : \"" + tmp0 + (String)"\",\\\n";
  html_tmp = html_tmp + (String)"\"lastUpdateMonth\" : \"" + tmp2 + (String)"\",\\\n";
  html_tmp = html_tmp + (String)"\"lastUpdateDay\" : \"" + tmp3 + (String)"\",\\\n";
  html_tmp = html_tmp + (String)"\"lastUpdateHour\" : \"" + tmp4 + (String)"\",\\\n";
  html_tmp = html_tmp + (String)"\"lastUpdateMin\" : \"" + tmp5 + (String)"\",\\\n";
  html_tmp = html_tmp + (String)"\"lastUpdateRetry\" : \"" + tmp6 + (String)"\",\\\n";
*/
// Display settings data
// DisplayFormat
  snprintf(temp, 3000,"\"dispFormat\" : %d,\\\n",confDat.GetdispFormatw());
  html_tmp = html_tmp + String(temp);

// TimeDisplayFormat
  snprintf(temp, 3000,"\"timeDisplayFormat\" : %d,\\\n",confDat.getTimeDisplayFormat());
  html_tmp = html_tmp + String(temp);

// DateDisplayFormat
  snprintf(temp, 3000,"\"dateDisplayFormat\" : %d,\\\n",confDat.getDateDisplayFormat());
  html_tmp = html_tmp + String(temp);

// formatHour
  snprintf(temp, 3000,"\"formatHour\": %d,\\\n",confDat.GetFormatHw());
  html_tmp = html_tmp + String(temp);

// DisplayEffect
  snprintf(temp, 3000,"\"displayEffect\" : %d,\\\n",confDat.getDisplayEffect());
  html_tmp = html_tmp + String(temp);

//  fadetime
  snprintf(temp, 3000,"\"fadeTime\" : %d,\\\n",confDat.GetFadetimew());
  html_tmp = html_tmp + String(temp);

//  brDig
  snprintf(temp, 3000,"\"brDig\" : [%d,%d,%d,%d,%d,%d,%d,%d,%d],\\\n"
  ,confDat.GetBr_dig(0), confDat.GetBr_dig(1), confDat.GetBr_dig(2), confDat.GetBr_dig(3)
  ,confDat.GetBr_dig(4), confDat.GetBr_dig(5), confDat.GetBr_dig(6), confDat.GetBr_dig(7)
  ,confDat.GetBr_dig(8)
  );
  html_tmp = html_tmp + String(temp);

// glowBright
//  snprintf(temp, 3000,"\"glowBright\" : %d,\\\n",confDat.GetGlowBright());
  snprintf(temp, 3000,"\"glowInTheBright\" : %d,\\\n",confDat.GetGlowInTheBright());
  html_tmp = html_tmp + String(temp);
// glowDark
//  snprintf(temp, 3000,"\"glowDark\" : %d,\\\n",confDat.GetGlowDark());
  snprintf(temp, 3000,"\"glowInTheDark\" : %d,\\\n",confDat.GetGlowInTheDark());
  html_tmp = html_tmp + String(temp);

// WiFi settings data
  snprintf(temp, 3000,"\"staAutoConnect\" : %d,\\\n",confDat.getStaAutoConnectSel());
  html_tmp = html_tmp + String(temp);

// 
  html_tmp = html_tmp + String("\"title\":\"Hello!\"\\\n");

  snprintf(temp, 3000,"}\';");
  html_tmp = html_tmp + String(temp);
  Serial.println(html_tmp);

  return(html_tmp);
}

// 初期化
SettingjsFile::SettingjsFile(void){
  fileWriteReq = 0;
  return;
}

void SettingjsFile::setWriteReq(uint8_t data)
{
  fileWriteReq = data;

  return;
}
uint8_t SettingjsFile::getWriteReq(void)
{
  return fileWriteReq;
}

//#ifdef DELETE
void SettingjsFile::writeJsonFile(void){
  Serial.println("== writeJsonFile ==");

  String configData = "";
  configData = String("{\n");

// configHeader
  configData = configData + makeJsonPiece("configHeader", confDat.getConfigHeader(), true, false);      // configHeader
  configData = configData + makeJsonPiece("configVersion", confDat.getVersion(), true, false);          // configVersion

// == navbar ==
// 地域設定
  configData = configData + makeJsonPiece("localesId", confDat.getLocalesId(), true, false);            // localesId

// == Time Conf ==
//  ntpSet
  configData = configData + makeJsonPiece("ntpSet", confDat.getNtpset(), true, false);                  // ntpSet

// タイムゾーン
  configData = configData + makeJsonPiece("timeZone", confDat.getTimeZoneData(), true, false);          // timeZone
  configData = configData + makeJsonPiece("timeZoneAreaId", confDat.getTimeZoneAreaId(), true, false);  // timeZoneAreaId
  configData = configData + makeJsonPiece("timeZoneId", confDat.getTimeZoneId(), true, false);          // timeZoneId

// 自動更新時刻
  uint8_t tmp1,tmp2;
  confDat.getAutoUpdatetime(&tmp1,&tmp2);
  configData = configData + makeJsonPiece("autoUpdateHour", tmp1, true, false);                         // autoUpdateHour
  configData = configData + makeJsonPiece("autoUpdateMin", tmp2, true, false);                          // autoUpdateMin

/*
// 最終更新時刻
  uint16_t tmp0;
  uint8_t tmp3,tmp4,tmp5,tmp6;
  confDat.getLastUpdate(&tmp0,&tmp2,&tmp3,&tmp4,&tmp5,&tmp6);
  configData = configData + makeJsonPiece("lastUpdateYear", tmp0, true, false);                      // lastUpdateYear
  configData = configData + makeJsonPiece("lastUpdateMonth", tmp2, true, false);                     // lastUpdateMonth
  configData = configData + makeJsonPiece("lastUpdateDay", tmp3, true, false);                       // lastUpdateDay
  configData = configData + makeJsonPiece("lastUpdateHour", tmp4, true, false);                      // lastUpdateHour
  configData = configData + makeJsonPiece("lastUpdateMin", tmp5, true, false);                       // lastUpdateMin
  configData = configData + makeJsonPiece("lastUpdateRetry", tmp6, true, false);                     // lastUpdateRetry
*/

// == Display Conf ==
  configData = configData + makeJsonPiece("dispFormat", confDat.GetdispFormatw(), true, false);               // dispFormat
  configData = configData + makeJsonPiece("timeDisplayFormat", confDat.getTimeDisplayFormat(), true, false);  // timeDisplayFormat
  configData = configData + makeJsonPiece("dateDisplayFormat", confDat.getDateDisplayFormat(), true, false);  // dateDisplayFormat
  configData = configData + makeJsonPiece("formatHour", confDat.GetFormatHw(), true, false);                  // formatHour
  configData = configData + makeJsonPiece("displayEffect", confDat.getDisplayEffect(), true, false);      // displayEffect
  configData = configData + makeJsonPiece("fadeTime", confDat.GetFadetimew(), true, false);               // fadetime
  configData = configData + makeJsonPiece("glowInTheDark", confDat.GetGlowInTheBright(), true, false);    // glowBright
  configData = configData + makeJsonPiece("glowInTheDark", confDat.GetGlowInTheDark(), true, false);      // glowDark

//  brDig
  std::vector<uint8_t> brDig;
  for(uint8_t i=0; i<DISP_KETAMAX ; i++){
    brDig.push_back(confDat.GetBr_dig(i));
  }
  configData = configData + makeJsonPiece("brDig", brDig, true, false);  // brDig

// 
  configData = configData + makeJsonPiece("title", (String)"VFD Clock for IV-21", false, false);   // タイトル

  configData = configData + ("}");

  Serial.println(configData);

  File fp = SPIFFS.open("/setting.json","w");
  if( fp ){
    fp.print(configData);
    fp.close();
    Serial.println("Flash Write End.");
  }
  else{
    Serial.println("Flash Write Error.");
  }

  Serial.println("[E]");
  return;
}
//#endif

/**
 * @brief jsonデータ作成:String
 * 
 * @param key     jsonのキー
 * @param value   jsonの値
 * @param connma  true:データの最後に「.」を追加する
 *                false:追加しない
 * @param includeBraces true:データを{}で囲む
 *                      false:囲まない
 * @return String 
 */
String SettingjsFile::makeJsonPiece(String key, String value ,bool connma, bool includeBraces)
{
  String ret = "\"" + key + "\":\"" + value + "\"";
  if (includeBraces) {
    ret = "{" + ret + "}";
  }
  if(connma == true){
    ret = ret + ",";
  }
  ret = ret + "\n";
  return ret;
}

/**
 * @brief jsonデータ作成:uint8_t
 * 
 * @param key     jsonのキー
 * @param value   jsonの値
 * @param connma  true:データの最後に「.」を追加する
 *                false:追加しない
 * @param includeBraces true:データを{}で囲む
 *                      false:囲まない
 * @return String 
 */
String SettingjsFile::makeJsonPiece(String key, uint8_t value ,bool connma, bool includeBraces)
{
  String ret = "\"" + key + "\":" + value;
  if (includeBraces) {
    ret = "{" + ret + "}";
  }
  if(connma == true){
    ret = ret + ",";
  }
  ret = ret + "\n";
  return ret;
}

/**
 * @brief jsonデータ作成:uint8_t
 * 
 * @param key     jsonのキー
 * @param value   jsonの値
 * @param connma  true:データの最後に「.」を追加する
 *                false:追加しない
 * @param includeBraces true:データを{}で囲む
 *                      false:囲まない
 * @return String 
 */
String SettingjsFile::makeJsonPiece(String key, std::vector<uint8_t> value ,bool connma, bool includeBraces)
{
  String ret = "\"" + key + "\":";
  ret = ret + "[";
  for(uint8_t i=0; i<value.size(); i++){
    ret = ret + value[i];
    if(i != value.size()-1){
      ret = ret + ",";
    }
  }
  ret = ret + "]";

  if (includeBraces) {
    ret = "{" + ret + "}";
  }
  if(connma == true){
    ret = ret + ",";
  }
  ret = ret + "\n";
  return ret;
}

/**
 * @brief jsonデータ作成:uint16_t
 * 
 * @param key     jsonのキー
 * @param value   jsonの値
 * @param connma  true:データの最後に「.」を追加する
 *                false:追加しない
 * @param includeBraces true:データを{}で囲む
 *                      false:囲まない
 * @return String 
 */
String SettingjsFile::makeJsonPiece(String key, uint16_t value ,bool connma, bool includeBraces)
{
  String ret = "\"" + key + "\":" + value;
  if (includeBraces) {
    ret = "{" + ret + "}";
  }
  if(connma == true){
    ret = ret + ",";
  }
  ret = ret + "\n";
  return ret;
}

/**
 * @brief jsonデータ作成:float
 * 
 * @param key     jsonのキー
 * @param value   jsonの値
 * @param connma  true:データの最後に「.」を追加する
 *                false:追加しない
 * @param includeBraces true:データを{}で囲む
 *                      false:囲まない
 * @return String 
 */
String SettingjsFile::makeJsonPiece(String key, float value ,bool connma, bool includeBraces)
{
  String ret = "\"" + key + "\":" + value;
  if (includeBraces) {
    ret = "{" + ret + "}";
  }
  if(connma == true){
    ret = ret + ",";
  }
  ret = ret + "\n";
  return ret;
}

#ifdef DELETE
void SettingjsFile::writeJsonFile(void){

  // ファイルにデータを書き込む
  File file = LittleFS.open("/setting.json","w");
  if (!file) {
    Serial.println("ファイルのオープンに失敗しました");
    return;
  }

  file.println("Hello, LittleFS!");
  file.println("ESP32でファイルシステムを使用中です。");
  file.close();
  Serial.println("ファイル書き込み完了");

  // 書き込んだ内容を確認
  File readFile = LittleFS.open("/setting.json", "r");
  if (!readFile) {
    Serial.println("ファイルの読み込みに失敗しました");
    return;
  }

  Serial.println("ファイル内容:");
  while (readFile.available()) {
    Serial.write(readFile.read());
  }
  readFile.close();
}
#endif

void SettingjsFile::readJsonFile(configVFD *confDat){

  Serial.println("-- SettingjsFile::readJsonFile --");

  File file = SPIFFS.open("/setting.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  else{
    String readStr;
    while(file.available()){
      readStr = file.readStringUntil('\n'); // 改行まで１行読み出し
      if( readStr.startsWith("\"") ){
        String strTmp;
//          Serial.println(readStr.length());
//          Serial.println(readStr.lastIndexOf(","));
//          Serial.println(readStr.lastIndexOf('\r'));
        if(readStr.endsWith(",\r")){
          readStr = readStr.substring(0, readStr.lastIndexOf(","));
        }
        else{
          readStr = readStr.substring(0, readStr.lastIndexOf('\r'));
        }
        strTmp = "{" + readStr + "}";
        Serial.println(strTmp);
        setJsonData(strTmp,confDat,0);
      }
    }
  }
  file.close();

  return;
}

extern int jsonWriteReqf; // JSONファイル書き込み要求
void SettingjsFile::setJsonData(String jsonStr, configVFD *confDat,uint8_t fileWriteReq){
//  bool dateUpdate = 0;
  StaticJsonDocument<256> doc;
//  DynamicJsonDocument doc(256);

  Serial.println("-- setJsonData");
  DeserializationError error = deserializeJson(doc, jsonStr);

  if (error) {
    // エラーの場合
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else{
    JsonVariant jsondata;

    Serial.println(jsonStr);

  // configHeader
    jsondata = doc["configHeader"];
    if(!jsondata.isNull()){
      uint8_t num = jsondata.as<unsigned int>();
      confDat->setConfigHeader(num);
      Serial.println(confDat->getConfigHeader());
    }

  // configVersion
    jsondata = doc["configVersion"];
    if(!jsondata.isNull()){
      const char* dat = jsondata.as<const char*>();
      confDat->setVersion((char*)dat);
      Serial.println(confDat->getVersion());
    }

  // stamodeSSID
//    Serial.println("-- stamodeSSID");
    jsondata = doc["stamodeSSID"];
    if(!jsondata.isNull()){
      const char* dat = jsondata.as<const char*>();
      confDat->setSsid((char*)dat);
//      Serial.println(confDat->getTimeZoneData());
      Serial.println(confDat->getSsid());
//      dateUpdate = 1;
    }
    // SSID Password取得
    jsondata = doc["stamodeSSIDPass"];
    if (!jsondata.isNull()) {
      const char* dat = jsondata.as<const char*>();
      confDat->setSsidPass((char*)dat);
//      dateUpdate = 1;
      Serial.print("stamodeSSIDPass : ");
      Serial.println(confDat->getSsidPass());

      // STA再接続要求セット
      wifiStaReconnect = 1;
    }

    // localesId
    jsondata = doc["localesId"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
//      Serial.println(num);
      confDat->setLocalesId(num);
//      confDat->i2cEepromWriteReq(ADR_localesId);
    }

  // Time Conf
    // ntpSet
    jsondata = doc["ntpSet"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
//    Serial.println("-- ntpSet");
//      Serial.println(num);
      confDat->setNtpset(num);
      sntpCont.init();  // SNTP初期化
//      confDat->i2cEepromWriteReq(ADR_ntp_setw);
    }

    // timeZoneAreaId
    jsondata = doc["timeZoneAreaId"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      confDat->setTimeZoneAreaId(num);
      Serial.println("-+- timeZoneAreaId -+-");
      Serial.println(confDat->getTimeZoneAreaId());
    }

    // timeZoneId
    jsondata = doc["timeZoneId"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      confDat->setTimeZoneId(num);
      Serial.println("-+- timeZoneId -+-");
      Serial.println(confDat->getTimeZoneId());
    }

    // timezone
    jsondata = doc["timeZone"];
    if(!jsondata.isNull()){
      uint8_t num = jsondata.as<unsigned int>();
      confDat->setTimeZoneData(num);
      setNtpTime();
//      i2cEeromWriteReqAdr[ADR_timeZoneAreaId]=true;
//      i2cEeromWriteReqAdr[ADR_timeZoneId]=true;
//      confDat->i2cEepromWriteReq(ADR_timeZone);
//      i2cEeromWriteReqAdr[ADR_timeZone]=true;
//      i2cEeromWriteReq = true;    // EEPROM更新要求　TimeZone関連情報まとめて更新する
      Serial.println("-+- timeZone -+-");
      Serial.println(num);
      Serial.println(confDat->getTimeZoneData());
    }

    // 自動更新時刻設定
//    Serial.println("-- ntpAutoUpdateHour");
    jsondata = doc["ntpAutoUpdateHour"];
    if(!jsondata.isNull()){
      Serial.println("ntpAutoUpdateHour");
      uint8_t hour = jsondata.as<uint8_t>();
      jsondata = doc["ntpAutoUpdateMin"];
      uint8_t min = jsondata.as<uint8_t>();
      Serial.println(hour);
      Serial.println(min);
      confDat->setAutoUpdatetime(hour,min);
//      confDat->i2cEepromWriteReq(ADR_autoUpdateHourw);
    }

    // NTP時刻取得
    jsondata = doc["ntpConnect"];
    if (!jsondata.isNull()) {
      if(jsondata.as<int>() == 1){
        setNtpTime();
      }
    }

  // Disp Conf
    // formatHour
    jsondata = doc["formatHour"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.println(num);
      confDat->SetFormatHw(num);
//      confDat->i2cEepromWriteReq(ADR_format_hw);
      Serial.println("formatHour");
    }

    // DisplayFormat
    jsondata = doc["dispFormat"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.println(num);
      confDat->SetdispFormatw(num);
//      confDat->i2cEepromWriteReq(ADR_dispFormat);
    }

    // timeDisplayFormat
    jsondata = doc["timeDisplayFormat"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.println(num);
      confDat->setTimeDisplayFormat(num);
//      confDat->i2cEepromWriteReq(ADR_timeDisplayFormat);
    }

    // dateDisplayFormat
    jsondata = doc["dateDisplayFormat"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.println(num);
      confDat->setDateDisplayFormat(num);
//      confDat->i2cEepromWriteReq(ADR_dateDisplayFormat);
    }


    // 表示効果　即時確定
    jsondata = doc["displayEffect"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("displayEffect:");
      Serial.println(num);
      confDat->setDisplayEffect(num);
//      confDat->i2cEepromWriteReq(ADR_displayEffect);
    }

    // fadeTime 即時確定
    jsondata = doc["fadeTime"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("fadetime:");
      Serial.println(num);
      confDat->SetFadetimew(num);
//      confDat->i2cEepromWriteReq(ADR_fadetime);
      if(jsonWriteReqf == 0){
        jsonWriteReqf = 1;  // JSONファイル書き込み要求
      }
    }

    // 表示輝度初期値設定
    jsondata = doc["glowInTheBright"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("glowInTheBright:");
      Serial.println(num);
      confDat->SetGlowInTheBrightTmp(num);
      confDat->submitGlowInTheBright();
    }
    // 表示輝度初期値設定
    jsondata = doc["glowInTheDark"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("glowInTheDark:");
      Serial.println(num);
      confDat->SetGlowInTheDarkTmp(num);
      confDat->submitGlowInTheDark();
    }

    // 表示輝度設定スライダー値設定
    jsondata = doc["glowInTheBrighttmp"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("glowInTheBrighttmp:");
      Serial.println(num);
      confDat->SetGlowInTheBrightTmp(num);
    }
    jsondata = doc["glowInTheDarktmp"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("glowInTheDarktmp:");
      Serial.println(num);
      confDat->SetGlowInTheDarkTmp(num);
    }

    // 表示輝度設定スライダー値書き込み
    jsondata = doc["glowInTheBrightSet"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("glowInTheBrightSet:");
      Serial.println(num);
      if(num == 1){
        confDat->submitGlowInTheBright();
//        confDat->i2cEepromWriteReq(ADR_glowInTheBright);
      }
    }
    jsondata = doc["glowInTheDarkSet"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("glowInTheDarkSet:");
      Serial.println(num);
      if(num == 1){
        confDat->submitGlowInTheDark();
//        confDat->i2cEepromWriteReq(ADR_glowInTheDark);
      }
    }

    // brDig
    jsondata = doc["brDig"];
    if (!jsondata.isNull()) {
      JsonArray array = jsondata.as<JsonArray>();
      Serial.print("array.size()=");
      Serial.println(array.size());
      for (uint8_t i = 0; i< array.size(); i++){
        uint8_t data = array[i];
        confDat->SetBr_digtmp(i,data);
//        confDat->SetBr_dig(i,data);
//        Serial.print("brDig[");
//        Serial.print(i);
//        Serial.print("]=");
//        Serial.println(data);
      }
//      confDat.eeromWrite();
//      settingjsFile.setWriteReq(1);
    }

    // resetBrSetting 各桁の個別輝度設定 リセット処理
    jsondata = doc["resetBrSetting"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.println("[resetBrSetting]");
      if(num == 1){
        confDat->resetTmp();
      }
    }

    // writeBrSetting 各桁の個別輝度設定 保存処理
    jsondata = doc["writeBrSetting"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.println("[writeBrSetting]");
      if(num == 1){
        confDat->writeTmp();
      }
    }


    // RTC設定
//    Serial.println("-- rtcSetYear");
    jsondata = doc["rtcSetYear"];
    if(!jsondata.isNull()){
      struct tm timeInfo;
      timeInfo.tm_year = jsondata.as<int>() - 1900;
      jsondata = doc["rtcSetMon"];
      timeInfo.tm_mon = jsondata.as<int>() - 1;
      jsondata = doc["rtcSetDay"];
      timeInfo.tm_mday = jsondata.as<int>();
      jsondata = doc["rtcSetHour"];
      timeInfo.tm_hour = jsondata.as<int>();
      jsondata = doc["rtcSetMin"];
      timeInfo.tm_min = jsondata.as<int>();
      jsondata = doc["rtcSetSec"];
      timeInfo.tm_sec = jsondata.as<int>();

      Serial.println("RTC設定");
      localTimeCont.timeSync(timeInfo);
      localTimeCont.timeDisp();
      ntpSetup = true;                // NTP時刻 → RTC設定要求
    }

  // WiFi Conf
    // STA自動接続有効
    jsondata = doc["staAutoConnect"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.print("staAutoConnect:");
      Serial.println(num);
      confDat->setStaAutoConnectSel(num);
    }

    // WiFiアクセスポイントのリストを取得
    jsondata = doc["getWifiStaList"];
    if (!jsondata.isNull()) {
      if(jsondata.as<int>() == 1){
        Serial.println("getWifiStaList1");
        getwifiStaListreq = 1;      // 取得要求セット
      }
    }
    // SSID取得
    jsondata = doc["ssid"];
    if (!jsondata.isNull()) {
      const char* s = jsondata.as<const char*>();
      Serial.print("ssid : ");
      Serial.println(s);
      confDat->setSsid((char*)s);
    }
    // SSID Password取得
    jsondata = doc["ssidPassword"];
    if (!jsondata.isNull()) {
      const char* s = jsondata.as<const char*>();
      Serial.print("ssidPassword : ");
      Serial.println(s);
      confDat->setSsidPass((char*)s);
      wifiStaReconnect = 1;     // STA再接続要求セット
    }

    // WebIF Control Data

    // websocket情報
    jsondata = doc["websocket"];
    if (!jsondata.isNull()) {
      const char* s = jsondata.as<const char*>();
      Serial.print("websocket : ");
      Serial.println(s);
      Serial.println(websocketConnect);
      websocketConnect = true;    // websocket開始フラグ 開始設定
      Serial.println(websocketConnect);
      vfdevent.eventLogSendSqf = EVENT_LOGSEND_E2ROM_PRE;  // イベントLOG送信要求 EEPROMログ送信準備
    }

  }

  // 設定ファイル更新
  if((fileWriteReq == 1)/* && (dateUpdate == 1)*/){
//    settingjsFile.setWriteReq(1);
  }

  return;
}
