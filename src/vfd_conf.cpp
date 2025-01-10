/**
 * @file vfd_conf.cpp
 * @author hayasita04@gmail.com
 * @brief 設定値作成・Logデータ管理
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "vfd_conf.h"
#include "vfd_eerom.h"
#include "jsdata.h"

//extern EEROM_CTRL eerom;
extern const struct I2CEEROM_DATA eeromList[];

configVFD::configVFD(void){
  Serial.println("configVFD::configVFD");
  portENTER_CRITICAL(&mutexEeromWrReq);
  for(uint16_t count=0;count<=0xFF;count++){
    i2cEeromWriteReqAdr[count] = false;
  }
  i2cEeromWriteReq = false;
  portEXIT_CRITICAL(&mutexEeromWrReq);

  tabNumber = 0;
  return;
}
void configVFD::init(void){
  Serial.println("configVFD::configVFD");
  portENTER_CRITICAL(&mutexEeromWrReq);
  for(uint16_t count=0;count<=0xFF;count++){
    i2cEeromWriteReqAdr[count] = false;
  }
  i2cEeromWriteReq = false;
  portEXIT_CRITICAL(&mutexEeromWrReq);

  tabNumber = 0;
  return;
}

/**
 * @brief ConfigData i2cEEPROM値読み出し
 * 
 */
void configVFD::i2cEepromRead(void){
  uint8_t data[32];
  uint8_t i;

  if(eerom.conectChk() != 0){     // eerom接続なし
//    Serial.println("I2C EEPROM device not found.");
    return;
  }
//  Serial.println("I2C EEPROM device found at address 0x50.");

  if(eerom.read(0x50,0x0000)!=EEROM_HEADER){
    eerom.allclear();                  // E2ROMデータクリア
    i2cEepromIni();                    // ヘッダ一致しない場合は初期化
  }
  else{

    eerom.sequentialRead(0x50,0x0000,data,32);

    for(i=0;i<32;i++){
      Serial.print(data[i] >> 4, HEX);                // 上位4ビットを16表示
      Serial.print(data[i] & 0x0F, HEX);              // 下位4ビット
      Serial.print(" ");                              // データ間にスペース入れ
    }
    Serial.println();

    configData.format_hw = data[ADR_format_hw];     // 時刻表示フォーマット 12/24H
    configData.dispFormat = data[ADR_dispFormat];   // 表示フォーマット設定
    configData.timeDisplayFormat = data[ADR_timeDisplayFormat];   // 時刻表示フォーマット設定
    configData.dateDisplayFormat = data[ADR_dateDisplayFormat];   // 日付表示フォーマット設定
    configData.displayEffect = data[ADR_displayEffect];           // 表示効果設定
    configData.fadetimew = data[ADR_fadetime];                    // クロスフェード時間
    configData.glowInTheBright = data[ADR_glowInTheBright];       // 全体輝度設定値：明
    configTmp.glowInTheBright = data[ADR_glowInTheBright];
    configData.glowInTheDark = data[ADR_glowInTheDark];           // 全体輝度設定値：暗
    configTmp.glowInTheDark = data[ADR_glowInTheDark];
    configData.br_dig[0] = data[ADR_br_dig0];                     // 0x02 - 0x0A 表示各桁輝度
    configData.br_dig[1] = data[ADR_br_dig1];
    configData.br_dig[2] = data[ADR_br_dig2];
    configData.br_dig[3] = data[ADR_br_dig3];
    configData.br_dig[4] = data[ADR_br_dig4];
    configData.br_dig[5] = data[ADR_br_dig5];
    configData.br_dig[6] = data[ADR_br_dig6];
    configData.br_dig[7] = data[ADR_br_dig7];

    eerom.sequentialRead(0x50,0x0020,data,32);
    for(i=0;i<32;i++){
      Serial.print(data[i] >> 4, HEX);                // 上位4ビットを16表示
      Serial.print(data[i] & 0x0F, HEX);              // 下位4ビット
      Serial.print(" ");                              // データ間にスペース入れ
    }
    Serial.println();

    configData.br_dig[8] = data[ADR_br_dig8 - 0x20];

    configData.ntp_setw = data[ADR_ntp_setw - 0x20];              // ntp使用選択情報設定
    configData.timeZoneAreaId = data[ADR_timeZoneAreaId - 0x20];  // タイムゾーンエリアID設定設定
    configData.timeZoneId = data[ADR_timeZoneId - 0x20];          // タイムゾーンID設定設定
    configData.timeZoneData = data[ADR_timeZone - 0x20];          // タイムゾーン設定設定
    configData.autoUpdateHourw = data[ADR_autoUpdateHourw - 0x20];
    configData.autoUpdateMinw = data[ADR_autoUpdateMinw - 0x20];  // 自動更新時刻設定

    configData.staAutoConnectSel = data[ADR_staAutoConnect - 0x20];   // STA自動接続有効無効

    configData.localesId = data[ADR_localesId - 0x20];                // 地域設定設定

    eerom.sequentialRead(0x50,0x0040,data,32);
//    configData.eeromEventStartNum = data[ADR_eeromEventStartNum - 0x40];  // EEPROMイベント情報開始番号
    configData.eeromEventDataNum = data[ADR_eeromEventDataNum - 0x40];    // EEPROMイベント情報開始番号
    configData.eeromEventWriteNum = data[ADR_eeromEventWriteNum - 0x40];  // EEPROMイベント書き込み番号
  }
  eeromTmpMake();
  eerom.dump(0x0000,0x00FF);

  return;
}

// i2cEEPROM書込処理
void configVFD::i2cEepromWrite(void){
  std::vector<uint16_t> eeromNum;
  if(i2cEeromWriteReq == true){
    Serial.println("configVFD::i2cEepromWrite");
    portENTER_CRITICAL(&mutexEeromWrReq);
    i2cEeromWriteReq = false;
//    uint16_t count;
    for(uint16_t count=0;count<=0xFF;count++){
      if(i2cEeromWriteReqAdr[count] == true){
//        Serial.println(count);
//        Serial.println("i2cEEPROM write Req!");
        i2cEeromWriteReqAdr[count]=false;
        eeromNum.push_back(count);
//        void (*func)(uint8_t) = eeromList[count].writefunc;
//        (*func)(count);
      }
    }
    portEXIT_CRITICAL(&mutexEeromWrReq);

    for (uint16_t count = 0; count < eeromNum.size(); count++) {
      Serial.print(eeromNum[count]);
      Serial.println(" : i2cEEPROM write Req!");
      void (*func)(uint8_t) = eeromList[eeromNum[count]].writefunc;
      (*func)(eeromNum[count]);
    }

    Serial.println("configVFD::i2cEepromWrite-END");

    Serial.println(F("-- writeJsonFile"));
    settingjsFile.writeJsonFile();
  }

  return;
}

/**
 * @brief EEPPROM書き込み要求設定
 * 
 * @param adr 
 */
void configVFD::i2cEepromWriteReq(uint16_t adr)
{
  portENTER_CRITICAL(&mutexEeromWrReq);
  i2cEeromWriteReqAdr[adr]=true;
  i2cEeromWriteReq = true;          // Reqはデータ設定後にセットする。
                                    // config情報は一度に複数カテゴリーのデータ更新行わないので、排他無くてもok
  portEXIT_CRITICAL(&mutexEeromWrReq);
  return;
}

// ConfigData i2cEEPROM初期値書込
void configVFD::i2cEepromIni(void)
{
  eerom.iniWrite(ADR_eerom_header);
  eerom.iniWrite(ADR_version);
  eerom.iniWrite(ADR_format_hw);
  eerom.iniWrite(ADR_dispFormat);
  eerom.iniWrite(ADR_timeDisplayFormat);
  eerom.iniWrite(ADR_dateDisplayFormat);
  eerom.iniWrite(ADR_displayEffect);
  eerom.iniWrite(ADR_fadetime);
  eerom.iniWrite(ADR_glowInTheBright);
  eerom.iniWrite(ADR_glowInTheDark);
  eerom.iniWrite(ADR_br_dig0);
  eerom.iniWrite(ADR_br_dig1);
  eerom.iniWrite(ADR_br_dig2);
  eerom.iniWrite(ADR_br_dig3);
  eerom.iniWrite(ADR_br_dig4);
  eerom.iniWrite(ADR_br_dig5);
  eerom.iniWrite(ADR_br_dig6);
  eerom.iniWrite(ADR_br_dig7);
  eerom.iniWrite(ADR_br_dig8);

  eerom.iniWrite(ADR_ntp_setw);
  eerom.iniWrite(ADR_timeZoneAreaId);
  eerom.iniWrite(ADR_timeZoneId);
  eerom.iniWrite(ADR_timeZone);
  eerom.iniWrite(ADR_autoUpdateHourw);
  eerom.iniWrite(ADR_autoUpdateMinw);
  eerom.iniWrite(ADR_localesId);

  eerom.iniWrite(ADR_staAutoConnect);

//  eerom.iniWrite(ADR_eeromEventStartNum);
  eerom.iniWrite(ADR_eeromEventDataNum);
  eerom.iniWrite(ADR_eeromEventWriteNum);

  return;
}
// Config TmpData作成
void configVFD::eeromTmpMake(void)
{
  configTmp.format_hw = configData.format_hw;
  configTmp.fadetimew = configData.fadetimew;
  for(uint8_t i=0; i<DISP_KETAMAX ; i++){
    configTmp.br_dig[i] = configData.br_dig[i];
  }
  configTmp.glowInTheBright = configData.glowInTheBright;
  configTmp.glowInTheDark = configData.glowInTheDark;

  return;
}
// ConfigDataTmpクリア
void configVFD::resetTmp(void)
{
  eeromTmpMake();
  return;
}
// ConfigDataTmp設定
void configVFD::writeTmp(void)
{
//  configData.format_hw = configTmp.format_hw;
//  configData.fadetimew = configTmp.fadetimew;
  for(uint8_t i=0; i<DISP_KETAMAX ; i++){
    configData.br_dig[i] = configTmp.br_dig[i];
  }
  i2cEepromWriteReq(ADR_br_dig0);

  return;
}

/*
void configVFD::eeromWrite(void)
{
//  int16_t adr = 0;

//  Serial.println("-- EEROM_Write --");
//  eeromDataout();
#ifdef DELETE
  HEAD.put(0,HeaderData);
  HEAD.end();
  HEAD.commit();

  eeromConfWrite();
#endif
  // FPIFFS
//  settingjsFileWrite();
//  settingjsFileWriteReq = 1;
  settingjsFile.setWriteReq(1);

  return;
}
*/
/*
void configVFD::eeromConfWrite(void)
{
//  Serial.println("-- EEROM_Write Conf --");

//  CONF.put(0,configData);
//  CONF.end();
//  CONF.commit();

  return;
}
*/
// ConfigData表示
void configVFD::eeromDataout(void)
{
  String strBuf = "";
  strBuf += String("== Config Data Start ==\n");
  // Header
  strBuf += String("- Header Data -\n");
  String strBufTmp = "EEROM_HEADER : 0x";
  strBufTmp += String(HeaderData.eerom_header,HEX);
  strBuf += strBufTmp + "\n";

  // SW Version
  strBuf += String("Ver.") + HeaderData.version + String("\n");

  strBuf += String("- Config_data -") + String("\n");

  // WiFi Station mode SSID
//  strBuf += String("WiFi Station mode SSID : ") + configData.staSsid + String("\n");
//  strBuf += String("WiFi Station mode SSID Password : ") + configData.staSsidPassword + String("\n");

  // 地域設定
  strBuf += String("localesId : ") + configData.localesId + String("\n");

  // NTP動作選択
  strBuf += String("ntp_setw : ") + configData.ntp_setw + String("\n");

  // TimeZone設定
  strBuf += String("TimeZone : ") + String(configData.timeZoneData) + String("\n");

  // TimeZoneAreaID
  strBuf += String("TimeZoneAreaID : ") + configData.timeZoneAreaId + String("\n");

  // TimeZoneId
  strBuf += String("TimeZoneId : ") + String(configData.timeZoneId) + String("\n");

  // autoUpdateHourw
  strBuf += String("autoUpdateHourw : ") + String(configData.autoUpdateHourw) + String("\n");

  // autoUpdateMinw
  strBuf += String("autoUpdateMinw : ") + String(configData.autoUpdateMinw) + String("\n");

  // Clock Display Config Data

  // Display format_hw
  strBuf += String("format_hw : ") + configData.format_hw + String("\n");

  // timeDisplayFormat
  strBuf += String("timeDisplayFormat : ") + configData.timeDisplayFormat + String("\n");

  // dateDisplayFormat
  strBuf += String("dateDisplayFormat : ") + configData.dateDisplayFormat + String("\n");

  // Display Format
  strBuf += String("DisplayFormat : ") + configData.dispFormat + String("\n");

  // Display Effect
  strBuf += String("displayEffect : ") + configData.displayEffect + String("\n");

  // Display Fadetime
  strBuf += String("fadetimew : ") + configData.fadetimew + String("\n");

  // glowInTheBright
  strBuf += String("glowInTheBright : ") + configData.glowInTheBright + String("\n");

  // ADR_glowInTheDark
  strBuf += String("ADR_glowInTheDark : ") + configData.glowInTheDark + String("\n");

  // VFD brightness EEPROM
  strBufTmp = String("br_dig[] : ");
  for(uint8_t i=0; i<DISP_KETAMAX ; i++){
    strBufTmp += configData.br_dig[i];
  }
  strBuf += strBufTmp + String("\n");

  //
  strBuf += String("staAutoConnectSel : ") + configData.staAutoConnectSel + String("\n");  

  // VFD brightness Temp
  strBufTmp = String("Config_tmp\nbr_dig[] : ");
  for(uint8_t i=0; i<DISP_KETAMAX ; i++){
    strBufTmp += configTmp.br_dig[i];
  }
  strBuf += strBufTmp + String("\n");

//  strBuf += String("eeromEventStartNum : ") + configData.eeromEventStartNum + String("\n");  
  strBuf += String("eeromEventDataNum : ") + configData.eeromEventDataNum + String("\n");  
  strBuf += String("eeromEventWriteNum : ") + configData.eeromEventWriteNum + String("\n");  

  strBuf += String("== Config Data End ==");
 
#ifdef ARDUINO_M5Stack_Core_ESP32
  M5.Lcd.setCursor(0,0);
  M5.Lcd.print(strBuf);
#endif
  Serial.println(strBuf);

  return;
}
// ConfigData初期値設定
// i2cEepromIni()と統合するべき
void configVFD::setConfigDefault(void)
{
  uint8_t count;

  strcpy(HeaderData.version,VERSION);

  for(count = 0;count <= 0x5F;count++){
    void (*func)(uint8_t) = eeromList[count].defSetConf;
    (*func)(count);
  }
  eeromTmpMake();

// EEPROM書き込みフラグクリア
  portENTER_CRITICAL(&mutexEeromWrReq);
  for(uint16_t count=0;count<=0xFF;count++){
    i2cEeromWriteReqAdr[count] = false;
  }
  i2cEeromWriteReq = false;
  portEXIT_CRITICAL(&mutexEeromWrReq);

  return;
}
// EEPROM header設定
void configVFD::setConfigHeader(uint8_t data)
{
  HeaderData.eerom_header = data;
  return;

}
// EEPROM header取得
uint8_t configVFD::getConfigHeader(void)
{
  return(HeaderData.eerom_header);
}
// version設定
void configVFD::setVersion(char *data)
{
  strcpy(HeaderData.version,data);
  return;

}
// version取得
char *configVFD::getVersion(void)
{
  return(HeaderData.version);
}

// WiFi Station mode SSID,Password削除
void configVFD::clearStaData(void)
{
  strcpy(configData.staSsid,"");
  strcpy(configData.staSsidPassword,"");
  return;
}

// WiFi Station mode SSID取得
char *configVFD::getSsid(void)
{
  return(configData.staSsid);
}
// WiFi Station mode SSID設定
void configVFD::setSsid(char *data)
{
  strcpy(configData.staSsid,data);
  return;
}

// WiFi Station mode SSID Password取得
char *configVFD::getSsidPass(void)
{
  return(configData.staSsidPassword);
}
// WiFi Station mode SSID Password設定
void configVFD::setSsidPass(char *data)
{
  strcpy(configData.staSsidPassword,data);

  return;
}


// 地域設定取得
uint8_t configVFD::getLocalesId(void)
{
  return(configData.localesId);
}
// 地域設定設定
void configVFD::setLocalesId(uint8_t data)
{
  configData.localesId = data;
  i2cEepromWriteReq(ADR_localesId);
  return;
}

// ntp使用選択情報設定値操作
uint8_t configVFD::getNtpset(void)        // ntp使用選択情報取得
{
  return(configData.ntp_setw);
}
void configVFD::setNtpset(uint8_t datw)   // ntp使用選択情報設定
{
  if((datw >=0) && (datw <=1)){
    configData.ntp_setw = datw;
    i2cEepromWriteReq(ADR_ntp_setw);
  }
  return;
}

// タイムゾーン設定取得

const char* gmt[] = {
"GMT+12:00","GMT+11:00","GMT+10:00","GMT+09:30",
"GMT+09:00","GMT+08:00","GMT+07:00","GMT+06:00",
"GMT+05:00","GMT+04:00","GMT+03:30","GMT+03:00",
"GMT+02:00","GMT+01:00","GMT-00:00","GMT-01:00",
"GMT-02:00","GMT-03:00","GMT-03:30","GMT-04:00",
"GMT-04:30","GMT-05:00","GMT-05:30","GMT-05:45",
"GMT-06:00","GMT-06:30","GMT-07:00","GMT-08:00",
"GMT-08:30","GMT-08:45","GMT-09:00","GMT-09:30",
"GMT-10:00","GMT-10:30","GMT-11:00","GMT-12:00",
"GMT-12:45","GMT-13:00","GMT-14:00"
};

const char *configVFD::getTimeZoneData(void)
{
//  return(configData.timeZoneData);
//  return("GMT-00:00");
  return(gmt[configData.timeZoneData]);
}
uint8_t configVFD::getTimeZoneDataIndex(void)
{
  return(configData.timeZoneData);
}
// タイムゾーン設定設定
void configVFD::setTimeZoneData(uint8_t data)
{
  configData.timeZoneData = data;
  i2cEepromWriteReq(ADR_timeZone);
  return;
}
#ifdef DELETE
// タイムゾーンID設定取得
char *configVFD::getTimeZoneId(void)
{
  return(configData.timeZoneId);
}
// タイムゾーンID設定設定
void configVFD::setTimeZoneId(char *data)
{
  strcpy(configData.timeZoneId,data);
  return;
}
#endif
// タイムゾーンID設定取得
uint8_t configVFD::getTimeZoneId(void)
{
  return(configData.timeZoneId);
}
// タイムゾーンID設定設定
void configVFD::setTimeZoneId(uint8_t data)
{
  configData.timeZoneId = data;
  return;
}

// タイムゾーンエリアID設定取得
uint8_t configVFD::getTimeZoneAreaId(void)
{
  return(configData.timeZoneAreaId);
}
// タイムゾーンエリアID設定設定
void configVFD::setTimeZoneAreaId(uint8_t datw)
{
  configData.timeZoneAreaId = datw;
  return;
}

// 自動更新時刻取得
void configVFD::getAutoUpdatetime(uint8_t *hourw, uint8_t *minw)
{
  portENTER_CRITICAL(&mutex_conf);
  *hourw = configData.autoUpdateHourw;
  *minw = configData.autoUpdateMinw;
  portEXIT_CRITICAL(&mutex_conf);
  return;
}

// 自動更新時刻設定
void configVFD::setAutoUpdatetime(uint8_t hourw, uint8_t minw)
{
  portENTER_CRITICAL(&mutex_conf);
  configData.autoUpdateHourw = hourw;
  configData.autoUpdateMinw = minw;
  portEXIT_CRITICAL(&mutex_conf);
  i2cEepromWriteReq(ADR_autoUpdateHourw);
  return;
}

// 時刻表示フォーマット設定値操作
uint8_t configVFD::GetFormatHw(void)
{
  return(configData.format_hw);
}
uint8_t configVFD::GetFormatHwTmp(void)
{
  return(configTmp.format_hw);
}
void configVFD::SetFormatHw(uint8_t fw)
{
  if((fw >= 0) && (fw <= 1)){
    configData.format_hw = fw;
    i2cEepromWriteReq(ADR_format_hw);
  }
  return;
}
void configVFD::SetFormatHwTmp(uint8_t fw)
{
  if((fw >= 0) && (fw <= 1)){
    configTmp.format_hw = fw;
  }
  return;
}
void configVFD::FormatHchg(void)          // 切替
{
  if(configTmp.format_hw == 1){
    configTmp.format_hw = 0;
  }
  else{
    configTmp.format_hw = 1;
  }

  return;
}

uint8_t configVFD::GetdispFormatw(void)      // 表示フォーマット設定情報取得
{
    return(configData.dispFormat);
}
void configVFD::SetdispFormatw(uint8_t mode)   // 表示フォーマット設定情報記録
{
//  if((mode >= 0) && (mode <= 9)){
    configData.dispFormat = mode;
    i2cEepromWriteReq(ADR_dispFormat);
    dispFormatUpdatef = ON;               // 表示フォーマット更新
//  }
  return;
}

uint8_t configVFD::getTimeDisplayFormat(void)   // 時刻表示フォーマット設定取得
{
  return(configData.timeDisplayFormat);
}
void configVFD::setTimeDisplayFormat(uint8_t mode)   // 時刻表示フォーマット設定記録
{
  configData.timeDisplayFormat = mode;
  i2cEepromWriteReq(ADR_timeDisplayFormat);
  return;
}
uint8_t configVFD::getDateDisplayFormat(void)   // 日付表示フォーマット設定取得
{
  return(configData.dateDisplayFormat);
}
void configVFD::setDateDisplayFormat(uint8_t mode)   // 日付表示フォーマット設定記録
{
  configData.dateDisplayFormat = mode;
  i2cEepromWriteReq(ADR_dateDisplayFormat);
  return;
}

uint8_t configVFD::getDisplayEffect(void)       // 表示効果取得
{
  return(configData.displayEffect);
}
void configVFD::setDisplayEffect(uint8_t dat)   // 表示効果設定
{
  configData.displayEffect = dat;
  i2cEepromWriteReq(ADR_displayEffect);
  return;
}

uint8_t configVFD::GetFadetimew(void)      // クロスフェード時間情報取得
{
    return(configData.fadetimew);
}
uint8_t configVFD::GetFadetimewTmp(void)   // クロスフェード時間tmp情報取得
{
    return(configTmp.fadetimew);
}
void configVFD::SetFadetimew(uint8_t tw)   // クロスフェード時間情報記録
{
  if((tw >= 0) && (tw <= 9)){
    configData.fadetimew = tw;
    i2cEepromWriteReq(ADR_fadetime);
  }
  return;
}
void configVFD::SetFadetimewTmp(uint8_t tw)   // クロスフェード時間情報記録
{
  if((tw >= 0) && (tw <= 9)){
    configTmp.fadetimew = tw;
  }
  return;
}
void configVFD::FadetimeAdd(void)          // クロスフェード時間＋
{
  if(configTmp.fadetimew < 9){
    configTmp.fadetimew ++;
  }
  return;
}
void configVFD::FadetimeDec(void)          // クロスフェード時間－
{
  if(configTmp.fadetimew > 0){
    configTmp.fadetimew --;
  }
  return;
}


uint8_t *configVFD::GetBr_digp(void)       // 輝度情報ポインタ取得
{
  return(configData.br_dig);
}
uint8_t *configVFD::GetBr_digTmpp(void)    // 輝度情報tmpポインタ取得
{
  return(configTmp.br_dig);
}

uint8_t configVFD::GetBr_dig(uint8_t adj_point)       // 輝度情報個別取得
{
  return(configData.br_dig[adj_point]);
}
uint8_t configVFD::GetBr_digtmp(uint8_t adj_point)       // 輝度情報個別取得
{
  return(configTmp.br_dig[adj_point]);
}
void configVFD::SetBr_dig(uint8_t adj_point,uint8_t brw) // 輝度情報個別設定
{
  if((brw >= BR_MIN) && (brw<=BR_MAX)&&(adj_point<DISP_KETAMAX)){
    configData.br_dig[adj_point] = brw;
  }

/*
  if((brw >= 1) && (brw<=9)){
    Config_data.br_dig[adj_point] = brw;
  }
*/
  return;
}
void configVFD::SetBr_digtmp(uint8_t adj_point,uint8_t brw) // 輝度情報個別設定
{
  if((brw >= BR_MIN) && (brw<=BR_MAX)&&(adj_point<DISP_KETAMAX)){
    configTmp.br_dig[adj_point] = brw;
  }

/*
  if((brw >= 1) && (brw<=9)){
    Config_data.br_dig[adj_point] = brw;
  }
*/
  return;
}

void configVFD::Br_digAdd(uint8_t adj_point)           // 輝度情報＋
{
  if (configTmp.br_dig[adj_point - ADJ_BR1] < BR_MAX) {
    configTmp.br_dig[adj_point - ADJ_BR1]++;
  }
  else {
    configTmp.br_dig[adj_point - ADJ_BR1] = BR_MAX;
  }

  return;
}
void configVFD::Br_digDec(uint8_t adj_point)                        // 輝度情報－
{
    if (configTmp.br_dig[adj_point - ADJ_BR1] > BR_MIN) {
      configTmp.br_dig[adj_point - ADJ_BR1]--;
    }
    else {
      configTmp.br_dig[adj_point - ADJ_BR1] = BR_MIN;
    }
  return;
}

uint8_t configVFD::GetGlowInTheBright(void)      // 全体輝度設定値：明情報取得
{
    return(configData.glowInTheBright);
}
uint8_t configVFD::GetGlowInTheDark(void)      // 全体輝度設定値：暗情報取得
{
    return(configData.glowInTheDark);
}
uint8_t configVFD::GetGlowInTheBrightTmp(void)        // 全体輝度設定値：明情報取得
{
    return(configTmp.glowInTheBright);
}
uint8_t configVFD::GetGlowInTheDarkTmp(void)          // 全体輝度設定値：暗情報取得
{
    return(configTmp.glowInTheDark);
}
void configVFD::SetGlowInTheBrightTmp(uint8_t data)   // 全体輝度設定値：明情報設定
{
  configTmp.glowInTheBright = data;
  return;
}
void configVFD::SetGlowInTheDarkTmp(uint8_t data)     // 全体輝度設定値：明情報設定
{
  configTmp.glowInTheDark = data;
  return;
}
void configVFD::submitGlowInTheBright(void)           // 全体輝度設定値：明情報記録
{
  configData.glowInTheBright = configTmp.glowInTheBright;
  i2cEepromWriteReq(ADR_glowInTheBright);
  return;
}
void configVFD::submitGlowInTheDark(void)             // 全体輝度設定値：明情報記録
{
  configData.glowInTheDark = configTmp.glowInTheDark;
  i2cEepromWriteReq(ADR_glowInTheDark);
  return;
}


uint8_t configVFD::getStaAutoConnectSel(void)           // STA自動接続有効無効取得
{
  return(configData.staAutoConnectSel);
}
void configVFD::setStaAutoConnectSel(uint8_t data)      // STA自動接続有効無効記録
{
  configData.staAutoConnectSel = data;
  i2cEepromWriteReq(ADR_staAutoConnect);
  return;
}

// イベント情報
/*
uint8_t configVFD::getEeromEventStartNum(void)          // EEPROMイベント情報開始番号取得
{
  return(configData.eeromEventStartNum);
}
void configVFD::setEeromEventStartNum(uint8_t data)      // EEPROMイベント情報開始番号記録
{
  configData.eeromEventStartNum = data;
  i2cEepromWriteReq(ADR_eeromEventStartNum);
  return;
}
*/

uint8_t configVFD::getEeromEventDataNum(void)           // EEPROMイベント情報個数取得
{
  return(configData.eeromEventDataNum);
}
void configVFD::setEeromEventDataNum(uint8_t data)      // EEPROMイベント情報個数記録
{
  configData.eeromEventDataNum = data;
  i2cEepromWriteReq(ADR_eeromEventDataNum);
  return;
}

uint8_t configVFD::getEeromEventWriteNum(void)           // EEPROMイベント書き込み番号取得
{
  return(configData.eeromEventWriteNum);
}
void configVFD::setEeromEventWriteNum(uint8_t data)      // EEPROMイベント書き込み番号記録
{
  configData.eeromEventWriteNum = data;
  i2cEepromWriteReq(ADR_eeromEventWriteNum);
  return;
}


void configVFD::setTabName(const char *data)
{
  tabName = (String)data;
  Serial.print("configVFD::setTabName:");
  Serial.println(tabName);

  if(strcmp(data,"dashbord") == 0){
    tabNumber = 0;
  }else if(strcmp(data,"TimeConf") == 0){
    tabNumber = 1;
  }else if(strcmp(data,"DispConf") == 0){
    tabNumber = 2;
  }else if(strcmp(data,"SensorConf") == 0){
    tabNumber = 3;
  }
  Serial.println(tabNumber);

  return;
}
String configVFD::getTabName(void)
{
  return tabName;
}
uint8_t configVFD::getTabNumber(void)
{
  return tabNumber;
}

void configVFD::debugI2cWritePr(void)
{
  uint16_t ADR;
  uint8_t data;
  char ascii[16];                         // ASCII文字表示用バッファ
  Serial.println("debugI2cWritePr");

  for (ADR = 0; ADR <= 0xFF; ADR++) {    // 全アドレス範囲をダンプ
    if ((ADR & 0x00FF) == 0x00) {                 // 256バイト毎に
      Serial.println();                          // 一行開ける
    }
    if ((ADR % 16) == 0) {                        // 行の先頭なら
//      printADR(ADR);                              // アドレス表示
      for (int n = 0; n <= 15; n++) {
        ascii[n] = ' ';                         // ASCII文字表示バッファを初期化
      }
    }
    data = i2cEeromWriteReqAdr[ADR];
    Serial.print(data >> 4, HEX);                // 上位4ビットを16表示
    Serial.print(data & 0x0F, HEX);              // 下位4ビット
    Serial.print(" ");                           // データ間にスペース入れ
    if ((data > 0x20) & (data < 0x7F)) {        // 表示可能文字なら
      ascii[ADR & 0x0F] = data;                 // 表示バッファに記録
    }
    if ((ADR % 16) == 15) {                       // 行末なら
      for (int n = 0; n <= 15; n++) {           // ASCIIキャラクタで表示
        Serial.print(ascii[n]);
      }
      Serial.println();                          // 改行
    }
  }

  return;

}

// eventlog
eventLog::eventLog(void){
#ifdef OLD_EVENTLOG
  eventSentPt = 0;        // 旧方式　イベントデータ送信ポインタ
#endif
  logSemapho = xSemaphoreCreateBinary();  // イベントLogバッファ記録用セマフォ初期化
  if(logSemapho == NULL){
    Serial.println("セマフォ取得失敗");
  }else{
    xSemaphoreGive( logSemapho );         // セマフォ解放
  }

  eventSntpSync = false;  // SNTP同期完了イベント要求 クリア
  eventAutoConSntpReq = false;      // 0x21:SNTP処理要求イベント要求初期化
//  eventWifiDiscon = false;          // 0x8D:WiFI切断イベント要求初期化

  eventBuf2EepromSqf = EVENT_BUF2EEROM_WAIT;  // イベントLogバッファEEPROMへ書き込みシーケンスフラグ初期化
  lastWriteTime = millis();         // 前回書き込み時間初期化
  getLocalTime(&lastWriteTimeEp);   // 前回書き込み時刻初期化

  eepromLogBufferMaxSize = 0;       // バッファに記録された最大数初期化
  eeromWriteDataNum = 0;            // イベントデータ書き込み数初期化
  eeromWriteNum = 0;                // イベント書き込みバッファ番号初期化

  eventLogSendSqf = EVENT_LOGSEND_WAIT;   // イベントLOG送信要求シーケンスフラグ 待機
  logBufferReadNum = 0;             // イベントLOGバッファ読み出し数初期化
  eeromReadDataNum = 0;             // イベントデータ読み出し数初期化
  StartBufReadNum = 0;              // 送信開始バッファ番号初期化
  return;
}

// イベントログ記録
void eventLog::setEventlog(uint8_t event,uint16_t* data)
{
  EVENT_LOG tmpData;
  struct tm timeInfo;//時刻を格納するオブジェクト

  Serial.print("// setEventlog : ");
  Serial.println(event);

// イベントLOGデータ作成
  getLocalTime(&timeInfo);
  tmpData.year = 1900 + timeInfo.tm_year;
  tmpData.month = timeInfo.tm_mon+1;
  tmpData.day = timeInfo.tm_mday;
  tmpData.hour = timeInfo.tm_hour;
  tmpData.min = timeInfo.tm_min;
  tmpData.sec = timeInfo.tm_sec;

  tmpData.event = event;

  if (data != nullptr) {
    //
    for(uint8_t i = 0; i<4; i++){
      tmpData.data[i] = data[i];
    }
  }
  else{
//    Serial.println("no Data.");
    for(uint8_t i = 0; i<4; i++){
      tmpData.data[i] = 0;
    }
  }

// 旧方式　イベントLOGを<vector>logに記録して、websocketで送信する。
// イベントLOG追加
// イベントデータを<vector>logに追加する。
// logがEVENT_LOGMAXより多くなったら、先頭（一番古いもの）を削除する。
// 一番古いlogを削除した場合は、イベントデータ送信ポインタを-1する。
// イベントデータ送信ポインタは、次に送信するイベントLOGのポインタ。
#ifdef OLD_EVENTLOG
  portENTER_CRITICAL(&mutex);
  log.push_back(tmpData);           // LOGを追加
  if(log.size() > EVENT_LOGMAX){
    log.erase(log.begin());         // 先頭のLOGを削除
    if(eventSentPt > 0){
      eventSentPt--;                // イベントデータ送信ポインタ-1
    }
  }
  portEXIT_CRITICAL(&mutex);
  Serial.println(log.size());
  Serial.println(eventSentPt);
#endif

  // 新方式
  bool ret = setEepromLogBuf(tmpData);         // EEPROM書き込みバッファにデータセット
  if(ret){  // EEPROM書き込みバッファがMAX
    eventBuf2EepromSqf = EVENT_BUF2EEROM_WRITE;
//    eventBuf2Eeprom();  // イベントLogバッファからEEPROMへ書き込み
  }

  return;
}

/**
 * @brief イベントLogバッファからEEPROMへ書き込み<br>
 *  EEPROM書き込みバッファをEEPROMに書き込んで空にする。<br>
 *  新方式　イベントLOGを<bector>eepromLogBufferに一時記録し、定期的にEEPROMに書き込み行う。<br>
 *  websocketで送信するときは、EEPROMと<bector>eepromLogBufferの内容を送信する。<br>
 * イベントLOGをEEPROMに記録する。<br>
 * 前回書き込みから4時間経過で書き込み行う<br>
 */
void eventLog::eventBuf2Eeprom(void)
{
  static unsigned long startTime;  // 開始時間を保存
  const uint8_t writeNumMax = E2R_PAGESIZE / EVENT_LOGSIZE; // 1度に書き込む最大イベントLOG数
  std::vector<EVENT_LOG> logTmp;          // イベント記録Tmp
  uint8_t writeNum;                       // 書き込みイベント数
//  const unsigned long EEPROM_WRITETIME = (1000 * 60 * 60 * 4);  // 4hour
  const unsigned long EEPROM_WRITETIME = (1000 * 60 * 60 * 2);  // 2hour
//  const unsigned long EEPROM_WRITETIME = (1000 * 60 * 5 * 1);   // 5min
//  const unsigned long EEPROM_WRITETIME = (1000 * 60 * 1 * 1);   // 1min

  if( (eventBuf2EepromSqf == EVENT_BUF2EEROM_WAIT)
   && (eepromLogBuffer.size() > 0)
   && ((millis() - lastWriteTime) > EEPROM_WRITETIME) ){
    eventBuf2EepromSqf = EVENT_BUF2EEROM_WRITE;
    if((millis() - lastWriteTime) > EEPROM_WRITETIME){
      Serial.println("時間経過でEEPROM書き込み");
      setEventlog(EVENT_EVENT_DELAYED_WRITE);     // // イベント遅延書き込み
    }
  }

  if(eventBuf2EepromSqf == EVENT_BUF2EEROM_WRITE){
    startTime = millis();  // 開始時間を保存
    eeromWriteDataNum = confDat.getEeromEventDataNum();   // EEPROMイベント情報個数取得
    eeromWriteNum = confDat.getEeromEventWriteNum();      // EEROMイベント書き込み番号取得
    
    eventBuf2EepromSqf = EVENT_BUF2EEROM_WLOOP;   // 書き込みループ実行中
  }
  if(eventBuf2EepromSqf == EVENT_BUF2EEROM_WLOOP){  // 書き込みループ実行中
    Serial.println("EVENT_BUF2EEROM_WLOOP"); 
    logTmp.clear();                       // クリア

    // ページ内書き込み可能数計算
    writeNum = eeromWriteNum % writeNumMax;
    writeNum = writeNumMax - writeNum;
    // 書き込み個数作成
    if(eepromLogBuffer.size() >= writeNum){
      writeNum = writeNum;
    }
    else{
      writeNum = eepromLogBuffer.size();
    }

    //LOGバッファ記録のところにもmutex排他する
    portENTER_CRITICAL(&mutexEeprom);
    std::copy(eepromLogBuffer.begin(), eepromLogBuffer.begin() + writeNum, std::back_inserter(logTmp));
    eepromLogBuffer.erase(eepromLogBuffer.begin(), eepromLogBuffer.begin() + writeNum); // 先頭のLOGを削除

    // LOGバッファ送信個数-writenum、送信待機中なら。
    if(eventLogSendSqf == EVENT_LOGSEND_STANDBY){
    // イベントLOG送信待機
      // イベントLOGバッファ読み出し数から。↑で削除された先頭バッファ数を引く。
      if(logBufferReadNum >= writeNum){
        // 最小0まで引く
        logBufferReadNum -= writeNum;
      }
      else{
        // 0以下になる場合は、バッファから読み出していないデータがEEPROMに書き込まれた。
        // マイナス分はEEPROM読み出し数に設定する。
        eeromReadDataNum = eeromReadDataNum + (writeNum - logBufferReadNum);
        logBufferReadNum = 0;
      }
      
      Serial.print("更新 : logBufferReadNum : ");
      Serial.println(logBufferReadNum);
      Serial.print("更新 : eepromLogBuffer.size() : ");
      Serial.println(eepromLogBuffer.size());
      Serial.print("更新 : eeromReadDataNum : ");
      Serial.println(eeromReadDataNum);
      Serial.print("更新不要 : eeromWriteNum : ");
      Serial.println(eeromWriteNum);

    }
    portEXIT_CRITICAL(&mutexEeprom);

    writeEepromEvent(logTmp,eeromWriteNum,writeNum);   // 先頭のLOGをEEPROM書き込み
    
    // EEPROMイベント情報個数更新
    if(eeromWriteDataNum >= (EVENTLOG_NUM-writeNum)){
      eeromWriteDataNum = EVENTLOG_NUM;
    }
    else{
      eeromWriteDataNum += writeNum;
    }
    
    // EEPROMイベント書き込み番号更新
    if(eeromWriteNum >= (EVENTLOG_NUM-writeNum)){
      eeromWriteNum -= (EVENTLOG_NUM-writeNum);
    }
    else{
      eeromWriteNum += writeNum;
    }

    if(eepromLogBuffer.size() == 0){        // イベントLOGバッファが空になったので終了する
      confDat.setEeromEventDataNum(eeromWriteDataNum);  // EEPROMイベント情報個数設定
      confDat.setEeromEventWriteNum(eeromWriteNum);     // EEROMイベント書き込み番号設定
      eventBuf2EepromSqf = EVENT_BUF2EEROM_WAIT;        // 待機

      Serial.print("Time : ");
      Serial.println((millis() - startTime));

//      eerom.dump(ADR_eventLogStart,ADR_eventLogStart+(0x10 * EVENTLOG_NUM));  // Debug EEROM表示
//      Serial.print("Time : ");
//      Serial.println((millis() - startTime));

      lastWriteTime = millis();         // 前回書き込み時間
      getLocalTime(&lastWriteTimeEp);   // 前回書き込み時刻
    }
  }

  return;
}

// イベントLogをEEPROMとバッファから読み出す
uint8_t eventLog::eventSend(struct eventQueData *queData)
{
  uint8_t readNum = 0;                // 読み出し個数

  if((eventLogSendSqf == EVENT_LOGSEND_E2ROM_PRE)   // EEPROMログ送信準備
   && (eventBuf2EepromSqf == EVENT_BUF2EEROM_WAIT)  // イベントLogバッファEEPROMへ書き込みシーケンス 待機
  ){  
    Serial.println("- イベントLOG送信要求　試験 -");
    eeromReadDataNum = confDat.getEeromEventDataNum();    // EEPROMイベント情報個数取得
    eeromWriteNum = confDat.getEeromEventWriteNum();  // EEROMイベント書き込み番号取得

    // イベント送信を開始するバッファ番号
    if(eeromReadDataNum == EVENTLOG_NUM){
      // リングバッファがFULL
      StartBufReadNum = eeromWriteNum;    // 一番古いデータは、次に上書きされる場所にある。
    }
    else{
      StartBufReadNum = 0;  // リングバッファがFULLになっていない場合は、先頭のデータが一番古い
    }
    Serial.print("start Num : ");
    Serial.println(StartBufReadNum);
    Serial.print("readdata Num : ");
    Serial.println(eeromReadDataNum);
      
    eventLogSendSqf = EVENT_LOGSEND_E2ROM_EXEC; //EEPROMログ送信実行
  }
  else if(eventLogSendSqf == EVENT_LOGSEND_E2ROM_EXEC){  //EEPROMログ送信実行
    uint16_t eeromAdr;
    uint8_t readData[EVENT_LOGSIZE * EVENT_LOGREADMAX];   // EEPROM シーケンシャルリードデータ格納用

    std::vector<EVENT_LOG> eventLog;
    EVENT_LOG tmpData;

    if(eeromReadDataNum != 0){    // 未送信データあり
      // 読み出し個数作成
      if(eeromReadDataNum >= EVENT_LOGREADMAX){ // 残データ数が一度に読み出すデータ量よりも多いか
        if( (EVENTLOG_NUM - StartBufReadNum) < EVENT_LOGREADMAX){
          readNum = (EVENTLOG_NUM - StartBufReadNum); // EEPROMアドレスの終わりまで読み出す
        }
        else{
          readNum = EVENT_LOGREADMAX;     // 一度に読み出す最大数を指定
        }
      }
      else{
        readNum = eeromReadDataNum;     // 未送信データ全て読み出し
      }
      // EEPROMアドレス作成
      eeromAdr = ADR_eventLogStart + (EVENT_LOGSIZE * StartBufReadNum);

      Serial.print("readEepromEvent : ");
      Serial.print(eeromAdr,HEX);
      Serial.print(" : ");
      Serial.print(readNum);
      Serial.print(" : ");
      Serial.print(eeromReadDataNum);

      // EEPROM読み出し
      eerom.sequentialRead(E2R_I2CADR, eeromAdr, readData, EVENT_LOGSIZE*readNum);    // 外付けEEPROMから16byte読み出し
      
      // 未送信データ数更新
      if(eeromReadDataNum < readNum){
        eeromReadDataNum = 0;   // 書き込み個数の変数を流用しているが、読み書き両方で変数必要なので、分割する。
      }
      else{
        eeromReadDataNum -= readNum;
      }
      // 先頭位置更新
      if(StartBufReadNum >= (EVENTLOG_NUM - readNum)){
        StartBufReadNum -= (EVENTLOG_NUM - readNum);
      }
      else{
        StartBufReadNum += readNum;
      }

      // 読み出しデータ処理
      for(uint8_t i=0;i<readNum;i++){
        queData->eventData[i].year = (((uint16_t)readData[i*EVENT_LOGSIZE+0])<<8) + ((uint16_t)readData[i*EVENT_LOGSIZE+1] );
        queData->eventData[i].month = readData[i*EVENT_LOGSIZE+2];
        queData->eventData[i].day = readData[i*EVENT_LOGSIZE+3];
        queData->eventData[i].hour = readData[i*EVENT_LOGSIZE+4];
        queData->eventData[i].min = readData[i*EVENT_LOGSIZE+5];
        queData->eventData[i].sec = readData[i*EVENT_LOGSIZE+6];

        queData->eventData[i].event = readData[i*EVENT_LOGSIZE+7];
        queData->eventData[i].data[0] = (((uint16_t)readData[i*EVENT_LOGSIZE+8])<<8) + ((uint16_t)readData[i*EVENT_LOGSIZE+9] );
        queData->eventData[i].data[1] = (((uint16_t)readData[i*EVENT_LOGSIZE+8])<<10) + ((uint16_t)readData[i*EVENT_LOGSIZE+11] );
        queData->eventData[i].data[2] = (((uint16_t)readData[i*EVENT_LOGSIZE+8])<<12) + ((uint16_t)readData[i*EVENT_LOGSIZE+13] );
        queData->eventData[i].data[3] = (((uint16_t)readData[i*EVENT_LOGSIZE+8])<<14) + ((uint16_t)readData[i*EVENT_LOGSIZE+15] );
      }
      queData->dataSize = readNum;
    }
    else{
      // 未送信データなし
      Serial.println("EEPROM EVENT All Send.");

      logBufferReadNum = 0;                     // バッファを先頭から読み出す
      eventLogSendSqf = EVENT_LOGSEND_BUF_EXEC; // イベントバッファログ送信実行
    }

  }
  else if(eventLogSendSqf == EVENT_LOGSEND_BUF_EXEC){
    // イベントバッファログ送信実行
    Serial.println("// EVENT_LOGSEND_BUF_EXEC //");

    // 読み出し個数作成。LOGバッファのサイズを上限
    if((eepromLogBuffer.size() - logBufferReadNum) < EVENT_LOGREADMAX){
      // 未読み出しデータが1度に読み出す最大イベントLOG数以下なら、残り全て読み出し
      readNum = eepromLogBuffer.size() - logBufferReadNum;
    }
    else{
      readNum = EVENT_LOGREADMAX;
    }
    Serial.print("readNum : ");
    Serial.println(readNum);
    Serial.print("logBufferReadNum : ");
    Serial.println(logBufferReadNum);
    Serial.print("eepromLogBuffer.size() : ");
    Serial.println(eepromLogBuffer.size());

    // LOGバッファからLOGデータをコピー
    for(uint8_t i=0;i<readNum;i++){
      queData->eventData[i].year = eepromLogBuffer[logBufferReadNum+i].year;
      queData->eventData[i].month = eepromLogBuffer[logBufferReadNum+i].month;
      queData->eventData[i].day = eepromLogBuffer[logBufferReadNum+i].day;
      queData->eventData[i].hour = eepromLogBuffer[logBufferReadNum+i].hour;
      queData->eventData[i].min = eepromLogBuffer[logBufferReadNum+i].min;
      queData->eventData[i].sec = eepromLogBuffer[logBufferReadNum+i].sec;

      queData->eventData[i].event = eepromLogBuffer[logBufferReadNum+i].event;
      queData->eventData[i].data[0] = eepromLogBuffer[logBufferReadNum+i].data[0];
      queData->eventData[i].data[1] = eepromLogBuffer[logBufferReadNum+i].data[1];
      queData->eventData[i].data[2] = eepromLogBuffer[logBufferReadNum+i].data[2];
      queData->eventData[i].data[3] = eepromLogBuffer[logBufferReadNum+i].data[3];
    }
    queData->dataSize = readNum;

    // 読み出し数カウンタ更新
    logBufferReadNum += readNum;

    // LOGバッファサイズ== LOGバッファ読み出し個数なら、送信待機へ 
    if(eepromLogBuffer.size() == logBufferReadNum){
      eventLogSendSqf = EVENT_LOGSEND_STANDBY;
    }
  }
  else if(eventLogSendSqf == EVENT_LOGSEND_STANDBY){
  // イベントLOG送信待機
    if( (eventBuf2EepromSqf != EVENT_BUF2EEROM_WRITE)
     && (eventBuf2EepromSqf != EVENT_BUF2EEROM_WLOOP) ){
      // EEPROM書き込み中は処理しない。
      if(eeromReadDataNum != 0){
        // EEPROM未読み出しイベント読み出し準備
        // eeromReadDataNum : 読み出すデータ数
        // StartBufReadNum : 読み出し開始位置

        Serial.println("-- EVENT_LOGSEND_STANDBY EEPROM --");
        Serial.print("eeromReadDataNum : ");
        Serial.println(eeromReadDataNum);
        Serial.print("eventBuf2EepromSqf : ");
        Serial.println(eventBuf2EepromSqf);

        // 読み出し位置作成
//        uint8_t eventDataNum = confDat.getEeromEventDataNum();    // EEPROMイベント情報個数取得
        uint8_t eventReadNum = confDat.getEeromEventWriteNum();   // EEROMイベント書き込み番号取得
        if(eventReadNum >= eeromReadDataNum){
          StartBufReadNum = eventReadNum - eeromReadDataNum;         // 読み出し位置作成
        }
        else{
          StartBufReadNum = (eventReadNum + EVENTLOG_NUM) - eeromReadDataNum;   // 読み出し位置作成
        }

//        Serial.print("eventDataNum : ");
//        Serial.println(eventDataNum);
        Serial.print("eventReadNum : ");
        Serial.println(eventReadNum,HEX);
        Serial.print("StartBufReadNum : ");
        Serial.println(StartBufReadNum,HEX);

        logBufferReadNum = 0;   //LOGバッファは全て読みだす
        eventLogSendSqf = EVENT_LOGSEND_E2ROM_EXEC;   //EEPROM読み出し
      }
      else{
        if(eepromLogBuffer.size() != logBufferReadNum){
          // eepromLogBufferにデータが追加されたら、バッファ読み出し処理実施
          Serial.println("-- EVENT_LOGSEND_STANDBY Buffer --");
          Serial.print("logBufferReadNum : ");
          Serial.println(logBufferReadNum);
          Serial.print("eepromLogBuffer.size() : ");
          Serial.println(eepromLogBuffer.size());
          Serial.print("eeromReadDataNum : ");
          Serial.println(eeromReadDataNum);
          Serial.print("eventBuf2EepromSqf : ");
          Serial.println(eventBuf2EepromSqf);

          eventLogSendSqf = EVENT_LOGSEND_BUF_EXEC;   //イベントバッファ読み出し
        }
      }
    }
//    eventLogSendSqf = EVENT_LOGSEND_WAIT;
  }

  return readNum;
}

// loopタスクイベントLogバッファ記録
void eventLog::setEventlogLoop(uint8_t event,uint16_t* data){
//  Serial.println("setEventlogLoop");
//  Serial.print(event);
  if (xSemaphoreTake(logSemapho, portMAX_DELAY) == pdTRUE) {  // セマフォ取得
    if (data != nullptr) {
      vfdevent.setEventlog(event,data);
    }
    else{
      vfdevent.setEventlog(event);
    }
    // セマフォを解放
    xSemaphoreGive(logSemapho);
  }
  return;
}

// DeviceCtrlタスクイベントLogバッファ記録
void eventLog::setEventlogDeviceCtrl(uint8_t event,uint16_t* data){
//  Serial.println("setEventlogDeviceCtrl");
//  Serial.print(event);
  if (xSemaphoreTake(logSemapho, portMAX_DELAY) == pdTRUE) {  // セマフォ取得
    if (data != nullptr) {
      vfdevent.setEventlog(event,data);
    }
    else{
      vfdevent.setEventlog(event);
    }
    // セマフォを解放
    xSemaphoreGive(logSemapho);
  }
  return;
}

// EEPROM書き込み用イベントバッファLog記録
// eepromバッファにイベントデータを積む。
// データ数がmaxになったら、true,それ以外はfalseを返す。

inline bool eventLog::setEepromLogBuf(EVENT_LOG eventLog){
  bool ret = false;
  Serial.print("setEepromLogBuf : ");
  Serial.print("/-/- Heap available : ");
  Serial.println(esp_get_free_heap_size());
  putLog(eventLog);

  portENTER_CRITICAL(&mutexEeprom);
  eepromLogBuffer.push_back(eventLog);
  portEXIT_CRITICAL(&mutexEeprom);

  Serial.println(eepromLogBuffer.size());

  // EEPROM書き込みバッファ最大数作成(debug表示用)
  eepromLogBufferMaxSize = std::max(eepromLogBufferMaxSize, (uint8_t)eepromLogBuffer.size());

  if(eepromLogBuffer.size() >= EVENT_LOGBUFMAX){
    ret = true;
  }
  return ret;
}

/**
 * @brief バッファの中でnumが指定したイベントを返す。データがあればtrue,無ければfalseを返す。データは破壊しない
 * 
 * @param eventLog 
 * @param num 
 * @return true データあり
 * @return false データ無し
 */
bool eventLog::getEepromLogBuf(EVENT_LOG *eventLog,uint8_t num){
  bool ret = false;

  if(eepromLogBuffer.size() > 0){
    *eventLog = eepromLogBuffer[0];
    ret = true;
  }

  return ret;
}


/**
 * @brief イベントをEEPROMに書き込む
 * 　ページサイズを最大数とするが、個数のチェックをしていない。
 * 　返値でオーバーを返す？ページサイズ超えても複数回で書き込みする？
 * 　呼び出し元でページサイズとなるようにチェックしているはず。
 * 
 * @param log イベントデータ
 * @param eepromWriteNum EEPROM書き込み番号（アドレス）
 * @param num 書き込みイベント数
 */
void eventLog::writeEepromEvent(std::vector<EVENT_LOG> log,uint8_t eepromWriteNum,uint8_t num){
  uint16_t eeromAdr;
  uint8_t testdata[E2R_PAGESIZE];

  eeromAdr = ADR_eventLogStart + (eepromWriteNum * EVENT_LOGSIZE);  // 書き込みアドレス計算
//#ifdef DELETE
  Serial.print("writeEepromEvent : ");
  Serial.print(eeromAdr,HEX);
  Serial.print(" : ");
  Serial.println(num);
//#endif  
  for(uint8_t i=0;i<num;i++){
//    putLog(log[i]);
    testdata[(i*EVENT_LOGSIZE)+0] = (uint8_t)((log[i].year >> 8) & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+1] = (uint8_t)(log[i].year & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+2] = log[i].month;
    testdata[(i*EVENT_LOGSIZE)+3] = log[i].day;
    testdata[(i*EVENT_LOGSIZE)+4] = log[i].hour;
    testdata[(i*EVENT_LOGSIZE)+5] = log[i].min;
    testdata[(i*EVENT_LOGSIZE)+6] = log[i].sec;
    testdata[(i*EVENT_LOGSIZE)+7] = log[i].event;
    testdata[(i*EVENT_LOGSIZE)+8] = (uint8_t)((log[i].data[0] >> 8) & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+9] = (uint8_t)(log[i].data[0] & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+10] = (uint8_t)((log[i].data[1] >> 8) & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+11] = (uint8_t)(log[i].data[1] & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+12] = (uint8_t)((log[i].data[2] >> 8) & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+13] = (uint8_t)(log[i].data[2] & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+14] = (uint8_t)((log[i].data[3] >> 8) & 0x00FF);
    testdata[(i*EVENT_LOGSIZE)+15] = (uint8_t)(log[i].data[3] & 0x00FF);
  }
//  Serial.print("start:");
  eerom.sequentialWrite(0x50,eeromAdr,testdata,EVENT_LOGSIZE*num);
//  Serial.println("end.");  
  return;
}

// EVENT_LOGを表示
void eventLog::putLog(EVENT_LOG log){
  Serial.print(log.year);
  Serial.print("/");
  Serial.print(log.month);
  Serial.print("/");
  Serial.print(log.day);
  Serial.print(" ");
  Serial.print(log.hour);
  Serial.print(":");
  Serial.print(log.min);
  Serial.print(":");
  Serial.print(log.sec);
  Serial.print(" ");
  Serial.println(log.event,HEX);

  return;
}


/**
 * @brief EEPROM書き込みバッファ数取得
 * 
 * @return uint8_t 
 */
uint8_t eventLog::getEepromLogBufferSize(void){
  return eepromLogBuffer.size();
}

/**
 * @brief EEPROM書き込みバッファ最大数取得
 * 
 * @return uint8_t 
 */
uint8_t eventLog::getEepromLogBufferMaxSize(void){
  return eepromLogBufferMaxSize;
}
 
/**
 * @brief 前回書き込み時刻取得
 * 
 * @return struct tm 
 */
struct tm eventLog::getLastWriteTimeEp(void){
  return lastWriteTimeEp;
}
