#include "jsdata.h"
#include "vfd_conf.h"
#include "vfd_rtc.h"
#include "vfd_wificnt.h"
#include <FS.h>
#include "SPIFFS.h"
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


void SettingjsFile::writeJsonFile(void){
  String configData = "";
  char temp[60];
  uint16_t tmp0;
  uint8_t tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;

  Serial.println("-- settingjsFileWrite --");

  configData = String("{\n");

// configHeader
    snprintf(temp, 60,"\"configHeader\" : %d,\n",confDat.getConfigHeader());
    configData = configData + String(temp);

//  version
    configData = configData + (String)"\"configVersion\" : \"" + String(confDat.getVersion()) + (String)"\",\n\n";

// == navbar ==
// 地域設定
    configData = configData + ((String)"\"localesId\" : \"" + confDat.getLocalesId() + (String)"\",\n");

// == Time Conf ==
//  ntpSet
    if(confDat.getNtpset() == 1){
      configData = configData + ("\n\"ntpSet\":true,\n");
    }
    else{
      configData = configData + ("\n\"ntpSet\":false,\n");
    }

// タイムゾーン
    configData = configData + ((String)"\"timeZone\" : \"" + confDat.getTimeZoneData() + (String)"\",\n");
    configData = configData + ((String)"\"timeZoneAreaId\" : \"" + confDat.getTimeZoneAreaId() + (String)"\",\n");
    configData = configData + ((String)"\"timeZoneId\" : \"" + confDat.getTimeZoneId() + (String)"\",\n");

// 自動更新時刻
    confDat.getAutoUpdatetime(&tmp1,&tmp2);
    configData = configData + ((String)"\"autoUpdateHour\" : \"" + tmp1 + (String)"\",\n");
    configData = configData + ((String)"\"autoUpdateMin\" : \"" + tmp2 + (String)"\",\n");

// 最終更新時刻
/*
    confDat.getLastUpdate(&tmp0,&tmp2,&tmp3,&tmp4,&tmp5,&tmp6);
    configData = configData + ((String)"\"lastUpdateYear\" : \"" + tmp0 + (String)"\",\n");
    configData = configData + ((String)"\"lastUpdateMonth\" : \"" + tmp2 + (String)"\",\n");
    configData = configData + ((String)"\"lastUpdateDay\" : \"" + tmp3 + (String)"\",\n");
    configData = configData + ((String)"\"lastUpdateHour\" : \"" + tmp4 + (String)"\",\n");
    configData = configData + ((String)"\"lastUpdateMin\" : \"" + tmp5 + (String)"\",\n");
    configData = configData + ((String)"\"lastUpdateRetry\" : \"" + tmp6 + (String)"\",\n");

    Serial.println("[1]");
*/
// == Display Conf ==
// DisplayFormat
    snprintf(temp, 60,"\n\"dispFormat\" : %d,",confDat.GetdispFormatw());
    configData = configData + (temp) + String("\n");

// TimeDisplayFormat
    snprintf(temp, 60,"\"timeDisplayFormat\" : %d,",confDat.getTimeDisplayFormat());
    configData = configData + (temp) + String("\n");

// DateDisplayFormat
    snprintf(temp, 60,"\"dateDisplayFormat\" : %d,",confDat.getDateDisplayFormat());
    configData = configData + (temp) + String("\n");

// formatHour
    snprintf(temp, 60,"\"formatHour\": %d,",confDat.GetFormatHw());
    configData = configData + (temp) + String("\n");

// DisplayEffect
    snprintf(temp, 60,"\"displayEffect\" : %d,",confDat.getDisplayEffect());
    configData = configData + (temp) + String("\n");

//  fadetime
    snprintf(temp, 60,"\"fadeTime\" : %d,",confDat.GetFadetimew());
    configData = configData + (temp) + String("\n");

// glowBright
    snprintf(temp, 60,"\"glowInTheBright\" : %d,",confDat.GetGlowInTheBright());
    configData = configData + (temp) + String("\n");

// glowDark
    snprintf(temp, 60,"\"glowInTheDark\" : %d,",confDat.GetGlowInTheDark());
    configData = configData + (temp) + String("\n");

//  brDig
    snprintf(temp, 60,"\"brDig\" : [%d,%d,%d,%d,%d,%d,%d,%d,%d],"
    ,confDat.GetBr_dig(0), confDat.GetBr_dig(1), confDat.GetBr_dig(2), confDat.GetBr_dig(3)
    ,confDat.GetBr_dig(4), confDat.GetBr_dig(5), confDat.GetBr_dig(6), confDat.GetBr_dig(7)
    ,confDat.GetBr_dig(8)
    );
//    fp.println(temp);
    configData = configData + (temp) + String("\n");

// 
    configData = configData + (String("\n\"title\":\"Hello World!\"\n"));

    configData = configData + ("}");

  File fp = SPIFFS.open("/setting.json","w");
  if( fp ){
    fp.print(configData);
    fp.close();
  }
  else{
    Serial.println("Flash Write Error");
  }

  Serial.println("[E]");
  return;
}


#ifdef DELETE
void SettingjsFile::writeJsonFile(void){
  char temp[60];
  uint16_t tmp0;
  uint8_t tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;

  Serial.println("-- settingjsFileWrite --");

  File fp = SPIFFS.open("/setting.json","w");
  if( fp ){
    fp.println("{");


// configHeader
    snprintf(temp, 60,"\"configHeader\" : %d,",confDat.getConfigHeader());
    fp.println(temp);

//  version
    fp.println((String)"\"configVersion\" : \"" + String(confDat.getVersion()) + (String)"\",\n");

//  StationMode IP Adress
    if(confDat.GetStaIp() != ""){
      fp.println((String)"\"stamodeIP\" : \"" + confDat.GetStaIp() + (String)"\",");
    }


// APMode SSID
    if(confDat.GetApSsid() != ""){
      fp.println((String)"\"atmodeSSID\" : \"" + confDat.GetApSsid() + (String)"\",");
    }

// APMode IP Adress
    if(confDat.GetApIp() != ""){
      fp.println((String)"\"atmodeIP\" : \"" + confDat.GetApIp() + (String)"\",\n");
    }

// == navbar ==
// 地域設定
    fp.println((String)"\"localesId\" : \"" + confDat.getLocalesId() + (String)"\",");

// == Time Conf ==
//  ntpSet
    if(confDat.getNtpset() == 1){
      fp.println("\n\"ntpSet\":true,");
    }
    else{
      fp.println("\n\"ntpSet\":false,");
    }

// タイムゾーン
    fp.println((String)"\"timeZone\" : \"" + confDat.getTimeZoneData() + (String)"\",");
    fp.println((String)"\"timeZoneAreaId\" : \"" + confDat.getTimeZoneAreaId() + (String)"\",");
    fp.println((String)"\"timeZoneId\" : \"" + confDat.getTimeZoneId() + (String)"\",");

// 自動更新時刻
    confDat.getAutoUpdatetime(&tmp1,&tmp2);
    fp.println((String)"\"autoUpdateHour\" : \"" + tmp1 + (String)"\",");
    fp.println((String)"\"autoUpdateMin\" : \"" + tmp2 + (String)"\",");

// 最終更新時刻
    confDat.getLastUpdate(&tmp0,&tmp2,&tmp3,&tmp4,&tmp5,&tmp6);
    fp.println((String)"\"lastUpdateYear\" : \"" + tmp0 + (String)"\",");
    fp.println((String)"\"lastUpdateMonth\" : \"" + tmp2 + (String)"\",");
    fp.println((String)"\"lastUpdateDay\" : \"" + tmp3 + (String)"\",");
    fp.println((String)"\"lastUpdateHour\" : \"" + tmp4 + (String)"\",");
    fp.println((String)"\"lastUpdateMin\" : \"" + tmp5 + (String)"\",");
    fp.println((String)"\"lastUpdateRetry\" : \"" + tmp6 + (String)"\",");

    Serial.println("[1]");

// == Display Conf ==
// DisplayFormat
    snprintf(temp, 60,"\n\"dispFormat\" : %d,",confDat.GetdispFormatw());
    fp.println(temp);

// TimeDisplayFormat
    snprintf(temp, 60,"\"timeDisplayFormat\" : %d,",confDat.getTimeDisplayFormat());
    fp.println(temp);

// DateDisplayFormat
    snprintf(temp, 60,"\"dateDisplayFormat\" : %d,",confDat.getDateDisplayFormat());
    fp.println(temp);

// formatHour
    snprintf(temp, 60,"\"formatHour\": %d,",confDat.GetFormatHw());
    fp.println(temp);

// DisplayEffect
    snprintf(temp, 60,"\"displayEffect\" : %d,",confDat.getDisplayEffect());
    fp.println(temp);

//  fadetime
    snprintf(temp, 60,"\"fadeTime\" : %d,",confDat.GetFadetimew());
    fp.println(temp);

// glowBright
    snprintf(temp, 60,"\"glowInTheBright\" : %d,",confDat.GetGlowInTheBright());
    fp.println(temp);

// glowDark
    snprintf(temp, 60,"\"glowInTheDark\" : %d,",confDat.GetGlowInTheDark());
    fp.println(temp);

//  brDig
    snprintf(temp, 60,"\"brDig\" : [%d,%d,%d,%d,%d,%d,%d,%d,%d],"
    ,confDat.GetBr_dig(0), confDat.GetBr_dig(1), confDat.GetBr_dig(2), confDat.GetBr_dig(3)
    ,confDat.GetBr_dig(4), confDat.GetBr_dig(5), confDat.GetBr_dig(6), confDat.GetBr_dig(7)
    ,confDat.GetBr_dig(8)
    );
    fp.println(temp);

// 
    fp.println(String("\n\"title\":\"Hello!\""));

    fp.print("}");

    fp.close();
  }
  else{
    Serial.println("Flash Write Error");
  }

  Serial.println("[E]");
  return;
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
