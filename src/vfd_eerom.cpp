#include "vfd_conf.h"
#include "vfd_eerom.h"
#include "vfd_driver.h"

//EEROM_CTRL eerom;

void test(uint8_t count){
  Serial.println("=-=-=- i2cEEROM Write Test -=-=-=");
}

void writeStdPr(uint8_t);
void writeAutoUpdateTime(uint8_t);
void writeBrDig(uint8_t);
void writeTimezone(uint8_t);
void writeDmyPr(uint8_t);

uint8_t getLocalesId(void);
uint8_t getNtpSet(void);
uint8_t getTimeZoneAreaId(void);
uint8_t getTimeZoneId(void);
uint8_t getTimeZoneDataIndex(void);

uint8_t getFormatHour(void);
uint8_t getDispFormat(void);
uint8_t getTimeDisplayFormat(void);
uint8_t getDatedisplayFormat(void);
uint8_t getDidsplayEffect(void);
uint8_t getFadetimeConf(void);
uint8_t getGlowInTheBright(void);
uint8_t getGlowInTheDark(void);
uint8_t getStaAutoConnectSel(void);

//uint8_t getEeromEventStartNumConf(void);
uint8_t getEeromEventDataNumConf(void);
uint8_t getEeromEventWriteNumConf(void);

void setDefDmyPr(uint8_t);

void setConfigHeader(uint8_t);
void setNtpSetDef(uint8_t prnum);
void setTimeZoneAreaIdDef(uint8_t prnum);
void setTimeZoneIdDef(uint8_t prnum);
void setTimeZoneIndexDef(uint8_t prnum);
void setLocalesIdDef(uint8_t);

void setFormatHourDef(uint8_t prnum);
void setDispFormatDef(uint8_t prnum);
void setTimeDisplayFormatDef(uint8_t prnum);
void setDatedisplayFormatDef(uint8_t prnum);
void setDidsplayEffectDef(uint8_t prnum);
void setFadetimeConfDef(uint8_t prnum);
void setGlowInTheBrightDef(uint8_t prnum);
void setGlowInTheDarkDef(uint8_t prnum);
void setBrDigDef(uint8_t prnum);

void setStaAutoConnectSelDef(uint8_t prnum);

//void setEeromEventStartNumDef(uint8_t);
void setEeromEventDataNumDef(uint8_t);
void setEeromEventWriteNumDef(uint8_t);

uint8_t getDmyPr(void){
  return 0;
}

extern const struct I2CEEROM_DATA eeromList[] = {
  {0x0000,  0x5A, 0x5A, 0x5A, &writeDmyPr, &getDmyPr, &setConfigHeader},    // eerom_header
  {0x0001,  '0',  0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},     // version
  {0x0002,  '.',  0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0003,  '0',  0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0004,  '1',  0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0005,  '.',  0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0006,  '0',  0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0007,  '0',  0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0008,  'S',  0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0009,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x000A,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},     // version
  {0x000B,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},    // no use
  {0x000C,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},    // no use
  {0x000D,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},    // no use
  {0x000E,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},    // no use
  {0x000F,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},    // no use

  {0x0010,  0x01, 0x00, 0xFF, &writeStdPr, &getFormatHour, &setFormatHourDef},    // format_hw = 1
  {0x0011,  0x00, 0x00, 0xFF, &writeStdPr, &getDispFormat, &setDispFormatDef},    // dispFormat = 0
  {0x0012,  0x01, 0x00, 0xFF, &writeStdPr, &getTimeDisplayFormat, &setTimeDisplayFormatDef},    // timeDisplayFormat
  {0x0013,  0x01, 0x00, 0xFF, &writeStdPr, &getDatedisplayFormat, &setDatedisplayFormatDef},    // dateDisplayFormat
  {0x0014,  0x00, 0x00, 0xFF, &writeStdPr, &getDidsplayEffect, &setDidsplayEffectDef},    // displayEffect
  {0x0015,  FADETIME_DEF, 0x00, 0xFF, &writeStdPr, &getFadetimeConf, &setFadetimeConfDef},    // fadetimew = FADETIME_DEF;    // クロスフェード時間初期値
  {0x0016,  0x52, 0x00, 0xFF, &writeStdPr, &getGlowInTheBright, &setGlowInTheBrightDef},    // glowInTheBright
  {0x0017,  0x48, 0x00, 0xFF, &writeStdPr, &getGlowInTheDark, &setGlowInTheDarkDef},    // glowInTheDark
  {0x0018,  BR_DEF, 0x00, 0xFF, &writeBrDig, &getDmyPr, &setBrDigDef},  // br_dig
  {0x0019,  BR_DEF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x001A,  BR_DEF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x001B,  BR_DEF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x001C,  BR_DEF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x001D,  BR_DEF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x001E,  BR_DEF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x001F,  BR_DEF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0020,  BR_DEF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0021,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0022,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0023,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0024,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0025,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0026,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0027,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0028,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0029,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x002A,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x002B,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x002C,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x002D,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x002E,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x002F,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},

  {0x0030,  0x01, 0x00, 0xFF, &writeStdPr, &getNtpSet, &setNtpSetDef},      // ntp_setw
  {0x0031,  0x04, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setTimeZoneAreaIdDef},       // timeZoneAreaId
  {0x0032,  0x50, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setTimeZoneIdDef},       // timeZoneId
  {0x0033,  0x1E, 0x00, 0xFF, &writeTimezone, &getDmyPr, &setTimeZoneIndexDef},    // timeZoneIndex
  {0x0034,  0x00, 0x00, 0xFF, &writeAutoUpdateTime, &getDmyPr, &setDefDmyPr},      // autoUpdateHourw
  {0x0035,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},      // autoUpdateMinw
  {0x0036,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0037,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0038,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0039,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x003A,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x003B,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x003C,  0x01, 0x00, 0xFF, &writeStdPr, &getLocalesId, &setLocalesIdDef},      // localesId
  {0x003D,  0x01, 0x00, 0xFF, &writeStdPr, &getStaAutoConnectSel, &setStaAutoConnectSelDef},      // staAutoConnect
  {0x003E,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x003F,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},

  {0x0040,  0x00, 0x00, 0x10, &writeStdPr, &getEeromEventDataNumConf, &setEeromEventDataNumDef},    // EEPROMイベント情報開始番号
  {0x0041,  0x00, 0x00, 0xFF, &writeStdPr, &getEeromEventWriteNumConf, &setEeromEventWriteNumDef},    // EEPROMイベント書き込み番号
  {0x0042,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0043,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0044,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0045,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0046,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0047,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0048,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0049,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x004A,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x004B,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x004C,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x004D,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x004E,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x004F,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},

  {0x0050,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0051,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0052,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0053,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0054,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0055,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0056,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0057,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0058,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x0059,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x005A,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x005B,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x005C,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x005D,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x005E,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},
  {0x005F,  0x00, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr},

  {0xFFFF,  0xFF, 0x00, 0xFF, &writeDmyPr, &getDmyPr, &setDefDmyPr}
};


void writeStdPr(uint8_t prnum){
  Serial.println("=-=-=- writeStdPr -=-=-=");

  uint8_t (*func)(void) = eeromList[prnum].readConf;
//  Serial.println((*func)());
//  Serial.println(prnum);

  Serial.printf("0x%x : %d\n",prnum,(*func)());

  eerom.write(0x50, prnum, (*func)());
  eerom.dump(0x0000,0x005F);

  return;
}

void writeAutoUpdateTime(uint8_t prnum){
  Serial.println("writeAutoUpdateTime");
  uint8_t hour,min;
  confDat.getAutoUpdatetime(&hour,&min);
  Serial.println(hour);
  Serial.println(min);

  eerom.write(0x50, ADR_autoUpdateHourw, hour);
  eerom.write(0x50, ADR_autoUpdateMinw, min);
  eerom.dump(0x0000,0x005F);
  return;
}

void writeBrDig(uint8_t prnum){
  eerom.write(0x50, ADR_br_dig0, confDat.GetBr_dig(0));
  eerom.write(0x50, ADR_br_dig1, confDat.GetBr_dig(1));
  eerom.write(0x50, ADR_br_dig2, confDat.GetBr_dig(2));
  eerom.write(0x50, ADR_br_dig3, confDat.GetBr_dig(3));
  eerom.write(0x50, ADR_br_dig4, confDat.GetBr_dig(4));
  eerom.write(0x50, ADR_br_dig5, confDat.GetBr_dig(5));
  eerom.write(0x50, ADR_br_dig6, confDat.GetBr_dig(6));
  eerom.write(0x50, ADR_br_dig7, confDat.GetBr_dig(7));
  eerom.write(0x50, ADR_br_dig8, confDat.GetBr_dig(8));
  eerom.dump(0x0000,0x005F);
  return;
}

void writeTimezone(uint8_t prnum){
  eerom.write(0x50, ADR_timeZoneAreaId, confDat.getTimeZoneAreaId());
  eerom.write(0x50, ADR_timeZoneId, confDat.getTimeZoneId());
  eerom.write(0x50, ADR_timeZone, confDat.getTimeZoneDataIndex());
  eerom.dump(0x0000,0x005F);
  return;
}

void writeDmyPr(uint8_t prnum){
  Serial.println("=-=-=- writeDummyPr -=-=-=");
  return;
}

//
// Config設定値読出関数
//

// Navbar
uint8_t getLocalesId(void){
  return confDat.getLocalesId();
}

// TimeConf
uint8_t getNtpSet(void){
  Serial.println("=-=-=- getNtpSet -=-=-=");
  Serial.println(confDat.getNtpset());
  return confDat.getNtpset();
}
uint8_t getTimeZoneAreaId(void){
  return confDat.getTimeZoneAreaId();
}
uint8_t getTimeZoneId(void){
  return confDat.getTimeZoneId();
}
uint8_t getTimeZoneDataIndex(void){
  return confDat.getTimeZoneDataIndex();
}



// DispConf
uint8_t getFormatHour(void){
  return confDat.GetFormatHw();
}
uint8_t getDispFormat(void){
  return confDat.GetdispFormatw();
}
uint8_t getTimeDisplayFormat(void){
  return confDat.getTimeDisplayFormat();
}
uint8_t getDatedisplayFormat(void){
  return confDat.getDateDisplayFormat();
}
uint8_t getDidsplayEffect(void){
  return confDat.getDisplayEffect();
}
uint8_t getFadetimeConf(void){
  return confDat.GetFadetimew();
}
uint8_t getGlowInTheBright(void){
  return confDat.GetGlowInTheBright();
}
uint8_t getGlowInTheDark(void){
  return confDat.GetGlowInTheDark();
}

// イベント記録
/*
uint8_t getEeromEventStartNumConf(void){
  return confDat.getEeromEventStartNum();
}
*/
uint8_t getEeromEventDataNumConf(void){
  return confDat.getEeromEventDataNum();
}
uint8_t getEeromEventWriteNumConf(void){
  return confDat.getEeromEventWriteNum();
}


//
// 初期値取得関数
//
void setDefDmyPr(uint8_t prnum){
//  Serial.print(prnum);
//  Serial.print(":");
//  Serial.println(eeromList[prnum].defaultNum);
  return;
}

void setConfigHeader(uint8_t prnum){
  confDat.setConfigHeader(eeromList[prnum].defaultNum);
  return;
}
void setNtpSetDef(uint8_t prnum){
  confDat.setNtpset(eeromList[prnum].defaultNum);
  return;
}
void setTimeZoneAreaIdDef(uint8_t prnum){
  confDat.setTimeZoneAreaId(eeromList[prnum].defaultNum);
  return;
}
void setTimeZoneIdDef(uint8_t prnum){
  confDat.setTimeZoneId(eeromList[prnum].defaultNum);
  return;
}
void setTimeZoneIndexDef(uint8_t prnum){
  confDat.setTimeZoneData(eeromList[prnum].defaultNum);
  return;
}
void setLocalesIdDef(uint8_t prnum){
  confDat.setLocalesId(eeromList[prnum].defaultNum);
  return;
}

void setFormatHourDef(uint8_t prnum){
  confDat.SetFormatHw(eeromList[prnum].defaultNum);
  return;
}
void setDispFormatDef(uint8_t prnum){
  confDat.SetdispFormatw(eeromList[prnum].defaultNum);
  return;
}
void setTimeDisplayFormatDef(uint8_t prnum){
  confDat.setTimeDisplayFormat(eeromList[prnum].defaultNum);
  return;
}
void setDatedisplayFormatDef(uint8_t prnum){
  confDat.setDateDisplayFormat(eeromList[prnum].defaultNum);
  return;
}
void setDidsplayEffectDef(uint8_t prnum){
  confDat.setDisplayEffect(eeromList[prnum].defaultNum);
  return;
}
void setFadetimeConfDef(uint8_t prnum){
  confDat.SetFadetimew(eeromList[prnum].defaultNum);
  return;
}
void setGlowInTheBrightDef(uint8_t prnum){
  confDat.SetGlowInTheBrightTmp(eeromList[prnum].defaultNum);
  confDat.submitGlowInTheBright();
  return;
}
void setGlowInTheDarkDef(uint8_t prnum){
  confDat.SetGlowInTheDarkTmp(eeromList[prnum].defaultNum);
  confDat.submitGlowInTheDark();
  return;
}
void setBrDigDef(uint8_t prnum){
  for (uint8_t i = 0; i< DISP_KETAMAX; i++){
    confDat.SetBr_dig(i,eeromList[prnum+i].defaultNum);
  }
  return;
}

// WiFiConf
uint8_t getStaAutoConnectSel(void){
  return confDat.getStaAutoConnectSel();
}
void setStaAutoConnectSelDef(uint8_t prnum){
  confDat.setStaAutoConnectSel(eeromList[prnum].defaultNum);
  return;
}

// イベント
/*
void setEeromEventStartNumDef(uint8_t prnum){
  confDat.setEeromEventStartNum(eeromList[prnum].defaultNum);
  return;
}
*/
void setEeromEventDataNumDef(uint8_t prnum){
  confDat.setEeromEventDataNum(eeromList[prnum].defaultNum);
  return;
}
void setEeromEventWriteNumDef(uint8_t prnum){
  confDat.setEeromEventWriteNum(eeromList[prnum].defaultNum);
  return;
}


// ----------


EEROM_CTRL::EEROM_CTRL()
{
/*
  uint16_t count;
  for(count=0;count<=0xFF;count++){
    i2cEeromWriteReqAdr[count] = false;
  }
  i2cEeromWriteReq = false;
*/
//  Wire.begin(SDA_PIN,SCL_PIN);
  return;
}
uint8_t EEROM_CTRL::conectChk(void)
{
  eeromConent = 1;
//  Wire.begin(SDA_PIN,SCL_PIN);
  Wire.beginTransmission(I2C_ADDRESS_24C32);
  eeromConent = Wire.endTransmission();
//  Serial.println("EEROM_CTRL::conectChk");
//  Serial.println(eeromConent);
  if (eeromConent == 0){
    Serial.println("I2C EEPROM device found at address 0x50.");
  }
  else{
    Serial.println("I2C EEPROM device not found.");
  }

  return eeromConent;
}
void EEROM_CTRL::dump(uint16_t startADR,uint16_t endADR)
{
//  uint16_t startADR = 0x0000;
//  uint16_t endADR = 0x0FFF;           // アドレス上限指定 (24LC32なら0x0FFF)
  uint16_t ADR;
  uint8_t datasq[16];
  char ascii[16];                         // ASCII文字表示用バッファ

  if (eeromConent != 0){
    Serial.println("I2C EEPROM device not found.");
    return;
  }

  Serial.println();
  Serial.println("EEPROM dump");
  Serial.println("ADDR  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F ASCII CHARACTER");

  for (ADR = startADR; ADR <= endADR; ADR+=16) {    // 全アドレス範囲をダンプ
    sequentialRead(0x50, ADR,datasq,16);    // 外付けEEPROMから16byte読み出し

    if ((ADR & 0x00FF) == 0x00) {                 // 256バイト毎に
      Serial.println();                          // 一行開ける
    }
    if ((ADR % 16) == 0) {                        // 行の先頭なら
      printADR(ADR);                              // アドレス表示
      for (int n = 0; n <= 15; n++) {
        ascii[n] = ' ';                         // ASCII文字表示バッファを初期化
      }
    }

    for(uint8_t j=0;j<16;j++){
      uint8_t data = datasq[j];
//      data = read(0x50, ADR);             // 外付けEEPROMから読み出し
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
  }
  Serial.println();                          // 改行

  return;
}
#ifdef DELETE
void EEROM_CTRL::dump_old(uint16_t startADR,uint16_t endADR)
{
//  uint16_t startADR = 0x0000;
//  uint16_t endADR = 0x0FFF;           // アドレス上限指定 (24LC32なら0x0FFF)
  uint16_t ADR;
  uint8_t data;
  char ascii[16];                         // ASCII文字表示用バッファ

  if (eeromConent != 0){
    Serial.println("I2C EEPROM device not found.");
    return;
  }

  Serial.println();
  Serial.println("EEPROM dump");
  Serial.println("ADDR  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F ASCII CHARACTER");

  for (ADR = startADR; ADR <= endADR; ADR++) {    // 全アドレス範囲をダンプ
    if ((ADR & 0x00FF) == 0x00) {                 // 256バイト毎に
      Serial.println();                          // 一行開ける
    }
    if ((ADR % 16) == 0) {                        // 行の先頭なら
      printADR(ADR);                              // アドレス表示
      for (int n = 0; n <= 15; n++) {
        ascii[n] = ' ';                         // ASCII文字表示バッファを初期化
      }
    }
    data = read(0x50, ADR);             // 外付けEEPROMから読み出し
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
#endif
void EEROM_CTRL::write(uint8_t i2cADR, uint16_t eeADR, uint8_t data ) {
  if (eeromConent != 0){
    Serial.println("I2C EEPROM device not found.");
    return;
  }
  
  Wire.beginTransmission(i2cADR);                // i2cアドレス指定
  Wire.write((int)(eeADR >> 8));                 // EEPROM内アドレス指定 MSB
  Wire.write((int)(eeADR & 0xFF));               // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(5);                                      // 書き込み完了待ち
}

void EEROM_CTRL::sequentialWrite(uint8_t i2cADR, uint16_t eeADR ,uint8_t *data,uint8_t dataNum){
  uint8_t i=0;
  if (eeromConent != 0){
    Serial.println("I2C EEPROM device not found.");
    return;
  }
  if(dataNum > E2R_PAGESIZE){
    Serial.println("I2C EEPROM writeData over.");
    return;
  }
  Serial.println("- sequentialWrite");
  
  Wire.beginTransmission(i2cADR);                // i2cアドレス指定
  Wire.write((uint8_t)(eeADR >> 8));                 // EEPROM内アドレス指定 MSB
  Wire.write((uint8_t)(eeADR & 0xFF));               // LSB
  Wire.endTransmission();
  for(i=0;i<dataNum;i++){
    Wire.write(data[i]);
  }
  Wire.endTransmission();
  delay(5);                                      // 書き込み完了待ち

  return;
}


void EEROM_CTRL::iniWrite(uint16_t eeADR){
  if (eeromConent != 0){
    Serial.println("I2C EEPROM device not found.");
    return;
  }
  
  write(0x50,eeADR,eeromList[eeADR].defaultNum);
  return;
}
 
uint8_t EEROM_CTRL::read(uint8_t i2cADR, uint16_t eeADR ) {
  uint8_t data;
  if (eeromConent != 0){
    Serial.println("I2C EEPROM device not found.");
    return 0;
  }
  
  Wire.beginTransmission(i2cADR);               // i2cアドレス指定
  Wire.write((int)(eeADR >> 8));                // EEPROM内アドレス指定 MSB
  Wire.write((int)(eeADR & 0xFF));              // LSB
  Wire.endTransmission();
  Wire.requestFrom(i2cADR, 1);                  // 1バイトデータリクエスト
  while (Wire.available()) {                    //
    data = Wire.read();                         // データ受信
  }
  return data;
}

void EEROM_CTRL::sequentialRead(uint8_t i2cADR, uint16_t eeADR ,uint8_t *data,uint8_t dataNum){
  uint8_t i=0;
  //uint8_t data0;
  if (eeromConent != 0){
    Serial.println("I2C EEPROM device not found.");
    return;
  }
  
  Wire.beginTransmission(i2cADR);               // i2cアドレス指定
  Wire.write((int)(eeADR >> 8));                // EEPROM内アドレス指定 MSB
  Wire.write((int)(eeADR & 0xFF));              // LSB
  Wire.endTransmission();
  Wire.requestFrom(i2cADR, dataNum);                  // 1バイトデータリクエスト
  while (Wire.available()) {                    //
    data[i] = Wire.read();
//    Serial.print(data[i] >> 4, HEX);                // 上位4ビットを16表示
//    Serial.print(data[i] & 0x0F, HEX);              // 下位4ビット
//    Serial.print(" ");                           // データ間にスペース入れ
    i++;
  }
  Serial.println();                         // データ受信

  return;
}

void EEROM_CTRL::printADR(uint16_t a) {         // アドレスを4桁表示
  String b = "";
  if (a < 0x1000) {
    b = "0";
  }
  if (a < 0x100) {
    b = "00";
  }
  if ( a < 0x10) {
    b = "000";
  }
  Serial.print(b);
  Serial.print(a, HEX);
  Serial.print(" ");
}

// E2ROMデータクリア
void EEROM_CTRL::allclear(void)
{
  uint16_t e2rAdr;  // E2ROMアドレス
  uint8_t data[E2R_PAGESIZE];

  Serial.println("EEROM_CTRL::allclear");

  for(uint8_t i=0;i<E2R_PAGESIZE;i++){
    data[i] = 0xFF;
  }

  for(e2rAdr;e2rAdr<E2R_SIZE;e2rAdr+=E2R_PAGESIZE){
    sequentialWrite(E2R_I2CADR,e2rAdr,data,E2R_PAGESIZE);
  }

  Serial.print("lastADR : ");
  Serial.println(e2rAdr);
  return;
}                     

