#include "vfd_disp.h"
#include "vfd_driver.h"
#include "vfd_conf.h"
#include "mode_ctrl.h"

#ifdef ARDUINO_M5Stick_C
TFT_eSprite tftSprite = TFT_eSprite(&M5.Lcd); // スプライト
#endif

DispCtr::DispCtr(void)
{
    dispTmp = new uint16_t[DISP_KETAMAX];
    piriodTmp = new uint8_t[DISP_KETAMAX];
    dispdataTmp = new uint16_t[DISP_KETAMAX];

    displayMode = MODE_STD_DISP;   // 起動時の表示モード設定
    lastDispMode = displayMode;    // 前回表示モード設定

#ifdef ARDUINO_M5Stick_C
  // LCD Setting
    M5.Lcd.fillScreen(BLACK);
    tftSprite.createSprite(M5.Lcd.width(), M5.Lcd.height());
    tftSprite.setTextSize(2);
#endif

    return;
}

void DispCtr::init(void)
{
//  timerMux = portMUX_INITIALIZER_UNLOCKED;  // タスク内の初期化に移動。このクラス以外でも使うので。
  uint8_t i;
/*
// SPI Pin setting
  vspi = new SPIClass(VSPI);        // SPI宣言
  vspi->begin(SCLK, -1, MOSI, SS);  //SCLK, MISO, MOSI, SS
  pinMode(SS, OUTPUT);              //VSPI SS
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
*/
// shiftout Pin setting
  pinMode(SPISS_PIN, OUTPUT);
  pinMode(SPISCLK_PIN, OUTPUT);
  pinMode(SPIMOSI_PIN, OUTPUT);

  digitalWrite(SPISS_PIN, LOW);            //pull SS slow to prep other end for transfer

  pinMode(VFD_BLANKING, OUTPUT);
  digitalWrite(VFD_BLANKING, LOW);

  brightness_ini();                    // 輝度情報初期化
  stdDispFormat = confDat.GetdispFormatw();           // 表示フォーマット取得
  lastStdDispFormat = stdDispFormat;

  for(i = 0; i < 9; i++){
    disp[i] = 0;            // 数値表示データ初期化
    disp_last[i] = 0;       // 前回数値表示データ初期化
    disp_fadecount[i] = 0;  // クロスフェードサイクルカウンタ初期化
  }

    return;
}

void DispCtr::dispFormatAdd(void)               // VFD表示フォーマット+1
{
  stdDispFormat++;
  return;
}
void DispCtr::dispFormatDec(void)               // VFD表示フォーマット-1
{
  stdDispFormat--;
  return;
}


void DispCtr::brightness_ini(void)
{
  uint8_t i;
  uint8_t tmpw;
  
  tmpw = 0x00;
  
  if(tmpw == 0x5a){
    // 輝度情報EEPROM読み出し
 //   brightness_eeprom_load();
  }
  else{
    // EEPROM 初期化
    
    // 輝度情報初期化
    for (i = 0; i < 9; i++) {
      brightness_dig[i] = confDat.GetBr_dig(i);
    }
//    brightness_eeprom_save();
  }

  return;
}

//void DispCtr::dataMake(struct tm timeInfo)
void DispCtr::dataMake(struct DISPLAY_DATA inputData)
{
  unsigned int i;
//  unsigned char disp_tmp[9];      // 各桁表示データ
//  unsigned char piriod_tmp[9];    // 各桁ピリオド
  unsigned long dispdata_tmp[9];  // 各桁表示データ(font情報)
  unsigned long dispdata;         // 表示データ作成用tmp
  uint16_t fadetime_tmpw;                     // クロスフェード時間受け渡し用データ
  uint8_t fade = ON;                          // クロスフェードON/OFF
  static uint8_t dispFormatWeb;         // WebIF設定の表示モード
  uint8_t dispFormat;                   // 表示フォーマット
  static unsigned long waitTime;        // 表示番号表示タイマ
  static uint8_t numDispSqf = 0;

  struct tm timeInfo = inputData.timeInfo;

  if(confDat.dispFormatUpdatef == ON){    // 修正必要　表示フォーマットの0番をweb設定表示、他は固定とする
    confDat.dispFormatUpdatef = OFF;
    dispFormatWeb = confDat.GetdispFormatw();           // 表示フォーマット取得
  }


  if(displayMode == MODE_STD_DISP){   // VFD表示モード
    if(stdDispFormat != lastStdDispFormat){
      if(numDispSqf == 0){
        waitTime = millis();
        numDispSqf = 1;
        Serial.println(numDispSqf);
      }
      else{
        dispNumber();
        if(millis() - waitTime > (unsigned long)500){
          lastStdDispFormat = stdDispFormat;
          numDispSqf = 0;
          Serial.println(numDispSqf);
        }
      }
    }else{
      if(stdDispFormat == 0){         // Web設定の表示
        dispFormat = dispFormatWeb;
      }
      else{
        dispFormat = stdDispFormat;
      }

      if(dispFormat == 1){
        dispClock(timeInfo);          // 時刻情報作成
      }
      else if(dispFormat == 2){
        dispCalender(timeInfo);       // 日付表示
      }
      else if(dispFormat == 3){
        dispLoop1(inputData);         // 時刻＋温度＋湿度＋気圧
      }
      else if(dispFormat == 4){
        dispTemp(inputData);          // 温度表示データ表示
      }
      else{
        dispClock(timeInfo);          // 時刻情報作成
      }
    }
  }
  else{   // VFD設定表示モード
    if(ctrlDispFormat == 0){
      if(ctrlModeSelect == 0){
        clockAdjtitleDispdatMake();   // タイトル表示
      }
      else{
        clockAdjDispdatMake(adjKeyData);        // 設定操作
      }
    }
  }
/*
  dispTmp[0] = DISP_00;
  dispTmp[1] = DISP_01;
  dispTmp[2] = DISP_02;
  dispTmp[3] = DISP_03;
  dispTmp[4] = DISP_04;
  dispTmp[5] = DISP_05;
  dispTmp[6] = DISP_06;
  dispTmp[7] = DISP_07;
  dispTmp[8] = DISP_08;
*/
  // 表示データ作成
  for (i = 0; i < 9; i++) {
    if (dispTmp[i] <= FONT_MAX) {
      dispdata = font[dispTmp[i]] | keta_dat[i];
      if (piriodTmp[i] != 0x00) {
        dispdata |= ELE_H;
      }
    }
    else{
      dispdata = font[DISP_NON];
    }
    dispdata_tmp[i] = dispdata;
  }

  // クロスフェード時間作成
  if(fade == ON){
//    fadetime_tmpw = confDat.GetFadetimewTmp() * 100;     // 設定用tmpから作成する
    fadetime_tmpw = confDat.GetFadetimew() * 100;
  }
  else{
    fadetime_tmpw = 0;                              // クロスフェードOFF
  }

  portENTER_CRITICAL(&timerMux);    // 割り込み禁止
  for (i = 0; i < 9; i++) {
    disp[i] = dispdata_tmp[i];                  // 表示データ
    brightness_dig[i]=confDat.GetBr_digtmp(i);  // 輝度情報
  }
  disp_fadetimei = fadetime_tmpw;               // クロスフェード時間受け渡し
  brp = confDat.GetBr_digTmpp();

  portEXIT_CRITICAL(&timerMux);     // 割り込み許可

  return;
}

// 表示番号表示データ作成
void DispCtr::dispNumber(void){
  dispTmp[0] = (uint16_t)DISP_NON;
  dispTmp[1] = (uint16_t)DISP_NON;
  dispTmp[2] = (uint16_t)DISP_NON;
  dispTmp[3] = (uint16_t)DISP_NON;
  dispTmp[4] = (uint16_t)DISP_NON;
  dispTmp[5] = (uint16_t)DISP_NON;
  dispTmp[6] = (uint16_t)stdDispFormat%10;
  dispTmp[7] = (uint16_t)stdDispFormat/10;
  dispTmp[8] = (uint16_t)DISP_NON;

  piriodTmp[0] = 0x00;
  piriodTmp[1] = 0x00;
  piriodTmp[2] = 0x00;
  piriodTmp[3] = 0x00;
  piriodTmp[4] = 0x00;
  piriodTmp[5] = 0x00;
  piriodTmp[6] = 0x01;
  piriodTmp[7] = 0x00;
  piriodTmp[8] = 0x00;

  return;
}

// 日付表示データ作成
void DispCtr::dispCalender(struct tm timeInfo)
{
  uint16_t tmpYear = timeInfo.tm_year + 1900;
  uint8_t tmpMonth = timeInfo.tm_mon + 1;
  uint8_t yearHh,yearHl,yearLh,yearLl;
  uint8_t monthH,monthL,dayH,dayL;

  dayL = timeInfo.tm_mday % 10;
  dayH = timeInfo.tm_mday / 10;
  monthL = tmpMonth % 10;
  monthH = tmpMonth / 10;
  yearLl = tmpYear % 10;
  yearLh = (tmpYear % 100) / 10;
  yearHl = (tmpYear % 1000) / 100;
  yearHh = tmpYear / 1000;

  if(confDat.getDateDisplayFormat() == dateDispFormYymmdd){
    // DateDisplay Format yy.mm.dd
    dispTmp[0] = dayL;
    dispTmp[1] = dayH;
    dispTmp[2] = monthL;
    dispTmp[3] = monthH;
    dispTmp[4] = yearLl;
    dispTmp[5] = yearLh;
    dispTmp[6] = DISP_NON;
    dispTmp[7] = DISP_NON;
  }
  else if(confDat.getDateDisplayFormat() == dateDispFormMmddyyyy){
    // DateDisplay Format mm.dd.yyyy
    dispTmp[0] = yearLl;
    dispTmp[1] = yearLh;
    dispTmp[2] = yearHl;
    dispTmp[3] = yearHh;
    dispTmp[4] = dayL;
    dispTmp[5] = dayH;
    dispTmp[6] = monthL;
    dispTmp[7] = monthH;
   }
  else if(confDat.getDateDisplayFormat() == dateDispFormMmddyy){
    // DateDisplay Format mm.dd.yy
    dispTmp[0] = yearLl;
    dispTmp[1] = yearLh;
    dispTmp[2] = dayL;
    dispTmp[3] = dayH;
    dispTmp[4] = monthL;
    dispTmp[5] = monthH;
    dispTmp[6] = DISP_NON;
    dispTmp[7] = DISP_NON;
  }
  else if(confDat.getDateDisplayFormat() == dateDispFormDdmmyyyy){
    // DateDisplay Format dd.mm.yyyy
    dispTmp[0] = yearLl;
    dispTmp[1] = yearLh;
    dispTmp[2] = yearHl;
    dispTmp[3] = yearHh;
    dispTmp[4] = monthL;
    dispTmp[5] = monthH;
    dispTmp[6] = dayL;
    dispTmp[7] = dayH;
  }
  else if(confDat.getDateDisplayFormat() == dateDispFormDdmmyy){
    // DateDisplay Format dd.mm.yy
    dispTmp[0] = yearLl;
    dispTmp[1] = yearLh;
    dispTmp[2] = monthL;
    dispTmp[3] = monthH;
    dispTmp[4] = dayL;
    dispTmp[5] = dayH;
    dispTmp[6] = DISP_NON;
    dispTmp[7] = DISP_NON;
  }
//  if(confDat.getDateDisplayFormat() == dateDispFormYyyymmdd){
  else{
    // DateDisplay Format yyyy.mm.dd
    dispTmp[0] = dayL;
    dispTmp[1] = dayH;
    dispTmp[2] = monthL;
    dispTmp[3] = monthH;
    dispTmp[4] = yearLl;
    dispTmp[5] = yearLh;
    dispTmp[6] = yearHl;
    dispTmp[7] = yearHh;
  }

  if( (confDat.getDateDisplayFormat() == dateDispFormYymmdd)  // DateDisplay Format yy.mm.dd
   || (confDat.getDateDisplayFormat() == dateDispFormMmddyy)  // DateDisplay Format mm.dd.yy
   || (confDat.getDateDisplayFormat() == dateDispFormDdmmyy)  // DateDisplay Format dd.mm.yy
  ){
    piriodTmp[0] = 0x01;
    piriodTmp[1] = 0x00;
    piriodTmp[2] = 0x01;
    piriodTmp[3] = 0x00;
    piriodTmp[4] = 0x01;
    piriodTmp[5] = 0x00;
    piriodTmp[6] = 0x00;
    piriodTmp[7] = 0x00;
    piriodTmp[8] = 0x00;
  }
  else if( (confDat.getDateDisplayFormat() == dateDispFormMmddyyyy) // DateDisplay Format mm.dd.yyyy
   || (confDat.getDateDisplayFormat() == dateDispFormDdmmyyyy)      // DateDisplay Format dd.mm.yyyy
  ){
    piriodTmp[0] = 0x01;
    piriodTmp[1] = 0x00;
    piriodTmp[2] = 0x00;
    piriodTmp[3] = 0x00;
    piriodTmp[4] = 0x01;
    piriodTmp[5] = 0x00;
    piriodTmp[6] = 0x01;
    piriodTmp[7] = 0x00;
    piriodTmp[8] = 0x00;
  }
  else{
    // DateDisplay Format yyyy.mm.dd
    piriodTmp[0] = 0x01;
    piriodTmp[1] = 0x00;
    piriodTmp[2] = 0x01;
    piriodTmp[3] = 0x00;
    piriodTmp[4] = 0x01;
    piriodTmp[5] = 0x00;
    piriodTmp[6] = 0x00;
    piriodTmp[7] = 0x00;
    piriodTmp[8] = 0x00;
  }

  return;
}
// 時計表示データ作成
void DispCtr::dispClock(struct tm timeInfo)
{
  uint8_t hour;
  uint8_t hourH,hourL,minH,minL,secH,secL;
  uint8_t ampm,piriodAmpm;

  hour = timeInfo.tm_hour;
  if(confDat.GetFormatHw() == 0){
    if(hour > 12){
      hour -= 12;
      ampm = DISP_P;
    }
    else{
      ampm = DISP_A;
    }
    piriodAmpm = 0x01;
  }
  else{
    ampm = DISP_NON;
    piriodAmpm = 0x00;
//    ampm = DISP_T;
//    piriodAmpm = 0x01;
  }

  secL = timeInfo.tm_sec % 10;
  secH = timeInfo.tm_sec / 10;
  minL = timeInfo.tm_min % 10;
  minH = timeInfo.tm_min / 10;
  hourL = hour % 10;
  if(((confDat.getTimeDisplayFormat() == timeDispFormHmmss))
   && ((hour / 10) == 0)){
    hourH = DISP_NON;
  }
  else{
    hourH = hour / 10;
  }

  if(confDat.getTimeDisplayFormat() == timeDispFormMmsshh){
    dispTmp[0] = (uint16_t)hourL;
    dispTmp[1] = (uint16_t)hourH;
    dispTmp[2] = (uint16_t)secL;
    dispTmp[3] = (uint16_t)secH;
    dispTmp[4] = (uint16_t)minL;
    dispTmp[5] = (uint16_t)minH;
  }
  else if((confDat.getTimeDisplayFormat() == timeDispFormHhmmss) || (confDat.getTimeDisplayFormat() == timeDispFormHmmss)){
    dispTmp[0] = (uint16_t)secL;
    dispTmp[1] = (uint16_t)secH;
    dispTmp[2] = (uint16_t)minL;
    dispTmp[3] = (uint16_t)minH;
    dispTmp[4] = (uint16_t)hourL;
    dispTmp[5] = (uint16_t)hourH;
  }

  dispTmp[6] = (uint16_t)DISP_NON;
  dispTmp[7] = (uint16_t)ampm;
  dispTmp[8] = (uint16_t)DISP_NON;

  piriodTmp[0] = 0x00;
  piriodTmp[1] = 0x00;
  piriodTmp[2] = 0x01;
  piriodTmp[3] = 0x00;
  piriodTmp[4] = 0x01;
  piriodTmp[5] = 0x00;
  piriodTmp[6] = 0x00;
  piriodTmp[7] = piriodAmpm;
  piriodTmp[8] = 0x00;

  return;  
}

// 温度表示データ表示
void DispCtr::dispTemp(struct DISPLAY_DATA inputData)
{
  uint16_t tempHH,tempHL,tempLH,tempLL;

  tempHH = ((inputData.temp % 10000) / 1000);
  tempHL = ((inputData.temp % 1000) / 100);
  tempLH = ((inputData.temp % 100) / 10);
  tempLL = (inputData.temp % 10);

  if(tempHH == 0){tempHH = DISP_NON;}
  if((tempHH == DISP_NON) && (tempHL == 0)){tempHL = DISP_NON;}

  dispTmp[7] = (uint16_t)DISP_T;
  dispTmp[6] = (uint16_t)DISP_NON;
  dispTmp[5] = (uint16_t)tempHH;
  dispTmp[4] = (uint16_t)tempHL;
  dispTmp[3] = (uint16_t)tempLH;
  dispTmp[2] = (uint16_t)tempLL;
  dispTmp[1] = (uint16_t)DISP_C;
  dispTmp[0] = (uint16_t)DISP_NON;

  piriodTmp[8] = 0x00;
  piriodTmp[7] = 0x01;
  piriodTmp[6] = 0x00;
  piriodTmp[5] = 0x00;
  piriodTmp[4] = 0x00;
  piriodTmp[3] = 0x01;
  piriodTmp[2] = 0x00;
  piriodTmp[1] = 0x00;
  piriodTmp[0] = 0x00;

  return;
}

// 湿度表示データ表示
void DispCtr::dispHum(struct DISPLAY_DATA inputData)
{
  uint16_t tempHH,tempHL,tempLH,tempLL;

  tempHH = ((inputData.humi % 10000) / 1000);
  tempHL = ((inputData.humi % 1000) / 100);
  tempLH = ((inputData.humi % 100) / 10);
  tempLL = (inputData.humi % 10);

  if(tempHH == 0){tempHH = DISP_NON;}
  if((tempHH == DISP_NON) && (tempHL == 0)){tempHL = DISP_NON;}

  dispTmp[7] = (uint16_t)DISP_H;
  dispTmp[6] = (uint16_t)DISP_NON;
  dispTmp[5] = (uint16_t)tempHH;
  dispTmp[4] = (uint16_t)tempHL;
  dispTmp[3] = (uint16_t)tempLH;
  dispTmp[2] = (uint16_t)tempLL;
  dispTmp[1] = (uint16_t)DISP_NON;
  dispTmp[0] = (uint16_t)DISP_NON;

  piriodTmp[8] = 0x00;
  piriodTmp[7] = 0x01;
  piriodTmp[6] = 0x00;
  piriodTmp[5] = 0x00;
  piriodTmp[4] = 0x00;
  piriodTmp[3] = 0x01;
  piriodTmp[2] = 0x00;
  piriodTmp[1] = 0x00;
  piriodTmp[0] = 0x00;

  return;
}

// 気圧表示データ作成
void DispCtr::dispPres(struct DISPLAY_DATA inputData)
{
  uint16_t tempHH,tempHL,tempLH,tempLL;

  tempHH = ((inputData.pressure % 10000) / 1000);
  tempHL = ((inputData.pressure % 1000) / 100);
  tempLH = ((inputData.pressure % 100) / 10);
  tempLL = (inputData.pressure % 10);

  if(tempHH == 0){tempHH = DISP_NON;}
  if((tempHH == DISP_NON) && (tempHL == 0)){tempHL = DISP_NON;}

  dispTmp[7] = (uint16_t)DISP_P;
  dispTmp[6] = (uint16_t)DISP_NON;
  dispTmp[5] = (uint16_t)tempHH;
  dispTmp[4] = (uint16_t)tempHL;
  dispTmp[3] = (uint16_t)tempLH;
  dispTmp[2] = (uint16_t)tempLL;
  dispTmp[1] = (uint16_t)DISP_NON;
  dispTmp[0] = (uint16_t)DISP_NON;

  piriodTmp[8] = 0x00;
  piriodTmp[7] = 0x01;
  piriodTmp[6] = 0x00;
  piriodTmp[5] = 0x00;
  piriodTmp[4] = 0x00;
  piriodTmp[3] = 0x00;
  piriodTmp[2] = 0x01;
  piriodTmp[1] = 0x00;
  piriodTmp[0] = 0x00;

  return;
}


void DispCtr::dispLoop1(struct DISPLAY_DATA inputData)   // Loop表示
{
  static uint8_t time = 0;
  static uint8_t lastSec = 0;

  if(lastSec != inputData.timeInfo.tm_sec){
    lastSec = inputData.timeInfo.tm_sec;
    time++;
  }

  if((time >= 0) && (time < 5)){
    dispClock(inputData.timeInfo);    // 時刻情報作成
  }
  else if((time >= 5) && (time < 8)){
    dispTemp(inputData);              // 温度表示データ表示
  }
  else if((time >= 8) && (time < 11)){
    dispHum(inputData);              // 温度表示データ表示
  }
  else if((time >= 11) && (time < 14)){
    dispPres(inputData);
  }

  if(time >= 14){
    time = 0;
  }

  return;
}

void DispCtr::dispLCD(struct tm timeInfo)
{
#ifdef ARDUINO_M5Stick_C
    tftSprite.fillScreen(BLACK);
    timeDispLcd(timeInfo);

    tftSprite.setCursor(0, 60);
    tftSprite.printf("%s",(confDat.getTabName()).c_str());
    if(confDat.getTabNumber() == 1){
        debugTimeConf();
    }
    else if(confDat.getTabNumber() == 2){
        debugDispConf();
    }

    tftSprite.pushSprite(0, 0);
#endif
    return;
}

/**
 * @brief 表示内容の切り替えを制御する
 * 
 * @param mode 表示モードの情報
 * @return uint8_t 操作モード更新のsoftKey入力
 */
uint8_t DispCtr::dispModeSet(dispMode mode)
{
  String status;
  uint8_t swKey = 0;

  if(mode.ctrlMode == ctrlMode_VfdCtrl){   // VFD設定
    displayMode = MODE_CLOCK_ADJ;
    ctrlDispFormat = mode.dispModeVfdCtrl;
    ctrlModeSelect = mode.ctrlModeSelect;
    adjKeyData = mode.adjKeyData;               // 設定操作用キー情報

    if(adjKeyData != 0){
      Serial.println(adjKeyData);
    }

    if(adjKeyData == KEY_SET_S){    // setキーで設定完了条件満たす場合の条件を追加
      swKey = SWKEY_SET_S;
      status = "設定モード脱出要求";
    }
  //  }else if(mode.ctrlMode == ctrlMode_VfdDisp){        // VFD表示
  }else{      // VFD表示 他
    if(mode.ctrlMode == ctrlMode_VfdDisp){    // VFD表示
      if((mode.adjKeyData == KEY_SET_S) && (mode.dispModeVfd != 0)){    // setキーで現在表示している表示を0番に設定
        confDat.SetdispFormatw(mode.dispModeVfd);                       // 表示フォーマット設定
        swKey = SWKEY_DISP_MODE_VFD_CLR;                                // VFD表示モード初期化要求
      }
      else{
      }
    }
    displayMode = MODE_STD_DISP;
    stdDispFormat = mode.dispModeVfd;
    ctrlModeSelect = mode.ctrlModeSelect;
    adjKeyData = 0;
  }

  if(displayMode != lastDispMode){     // モード変更あり
    lastDispMode = displayMode;        // 前回モード = 今回モード
    dispScrolldatMakeIni();         // スクロール表示データ初期化
//    dispBlinkingMakeIni();          // 表示データ点滅初期化
  }

  if(status.length() != 0){
    Serial.println(status);
  }

  return swKey;
}


/* 表示スクロールデータ作成 */
void DispCtr::dispScrolldatMakeIni(){
  disp_point = 0;
  scroll_tim_nowl = millis();
  return;
}

/*
  const char *disp_data : 表示テキストデータ
  uint8_t startp : 表示開始位置
  uint8_t dispnum : スクロール領域文字数
*/
void DispCtr::dispScrolldatMake(const char *disp_data,uint8_t startp,uint8_t dispnum) {
  unsigned char tmp;
  unsigned char disp_tmpp;
  long scrool_wait;

  // オーバーフロー確認
  if(startp > 7){
    startp = 7;
  }
  if(dispnum > startp){
    dispnum = startp;
  }

  // スクロール速度作成
  if(disp_point == 0){
    scrool_wait = 0;
  }
  else if(disp_point == 1){
    scrool_wait = 2000;
  }
  else{
    scrool_wait = 700;
  }

  // スクロールデータ作成
  if( ( millis() - scroll_tim_nowl ) > scrool_wait){
    for(unsigned char i=0 ; i<=dispnum ; i++){
      disp_tmpp = startp-i;

      if((disp_point + i) < strlen(disp_data)){
        if( (disp_data[disp_point+i] >= '0') && (disp_data[disp_point+i] <= '9')){
          tmp = disp_data[disp_point+i] - '0' + DISP_00;
        }
        else if( (disp_data[disp_point+i] >= 'A') && (disp_data[disp_point+i] <= 'Z')){
          tmp = disp_data[disp_point+i] - 'A' + DISP_A;
        }
        else if(disp_data[disp_point+i] == ' '){
          tmp = disp_data[disp_point+i] - ' ' + DISP_NON;
        }
        else{
          tmp = DISP_NON;
        }
        dispTmp[disp_tmpp] = tmp;
      }
      else{
        dispTmp[disp_tmpp] = DISP_NON;
      }
      piriodTmp[disp_tmpp] = 0x00;
    }
    disp_point++;
    if(disp_point > strlen(disp_data) ){
      disp_point = 0;
    }
    scroll_tim_nowl = millis();     // 前回スクロール時刻更新
  }

  return;
}

/**
 * @brief 時刻設定タイトル表示
 * 
 */
void DispCtr::clockAdjtitleDispdatMake(void){

  const char disptxt[] = "CLOCK SET";
  dispScrolldatMake(disptxt,5,5);
  dispTmp[6] = DISP_NON;
  dispTmp[7] = DISP_01;
  dispTmp[8] = DISP_K1;
  piriodTmp[7] = 0x01;

  return;
}

/**
 * @brief 時刻設定操作表示
 * 
 * @param adjKeyData 操作キー入力
 */
void DispCtr::clockAdjDispdatMake(uint8_t adjKeyData){

  const char disptxt[] = "CLOCK ADJ";
  dispScrolldatMake(disptxt,5,5);
  dispTmp[6] = DISP_NON;
  dispTmp[7] = DISP_01;
  dispTmp[8] = DISP_K1;
  piriodTmp[7] = 0x01;

  return;
}

#ifdef ARDUINO_M5Stick_C
void DispCtr::debugTimeConf(void)
{
    tftSprite.setCursor(0, 80);
    tftSprite.printf("TimeZone : %s\n",confDat.getTimeZoneData());

    return;
}

void DispCtr::debugDispConf(void)
{

    tftSprite.setCursor(0, 80);
    tftSprite.printf("%d : %d : %d\n",confDat.GetdispFormatw(),confDat.getDisplayEffect(),confDat.GetFadetimew());

    tftSprite.setCursor(0, 100);
    tftSprite.printf("B:%d D:%d\n",confDat.GetGlowInTheBright(),confDat.GetGlowInTheDark());
    tftSprite.setCursor(0, 120);
    tftSprite.printf("B:%d D:%d\n",confDat.GetGlowInTheBrightTmp(),confDat.GetGlowInTheDarkTmp());

    tftSprite.setCursor(0, 140);
    tftSprite.printf("%02d:%02d:%02d:%02d\n%02d:%02d:%02d:%02d\n%02d\n"
    ,confDat.GetBr_dig(0), confDat.GetBr_dig(1), confDat.GetBr_dig(2), confDat.GetBr_dig(3)
    ,confDat.GetBr_dig(4), confDat.GetBr_dig(5), confDat.GetBr_dig(6), confDat.GetBr_dig(7)
    ,confDat.GetBr_dig(8));

    tftSprite.setCursor(0, 190);
    tftSprite.printf("%02d:%02d:%02d:%02d\n%02d:%02d:%02d:%02d\n%02d\n"
    ,confDat.GetBr_digtmp(0), confDat.GetBr_digtmp(1), confDat.GetBr_digtmp(2), confDat.GetBr_digtmp(3)
    ,confDat.GetBr_digtmp(4), confDat.GetBr_digtmp(5), confDat.GetBr_digtmp(6), confDat.GetBr_digtmp(7)
    ,confDat.GetBr_digtmp(8));

    return;
}
void DispCtr::timeDispLcd(struct tm timeInfo)
{
  uint8_t hour;
  String ampm;

  hour = timeInfo.tm_hour;
  if(confDat.GetFormatHw() == 0){
    if(hour > 12){
        hour -= 12;
        ampm = "PM";
    }
    else{
        ampm = "AM";
    }
  }
  else{
        ampm = "--";
  }

  tftSprite.setCursor(0, 5);
  tftSprite.printf("%04d-%02d-%02d\n",timeInfo.tm_year + 1900, timeInfo.tm_mon + 1,timeInfo.tm_mday);
  tftSprite.setCursor(0, 25);
  tftSprite.printf("%02d:%02d:%02d %s\n",hour, timeInfo.tm_min, timeInfo.tm_sec, ampm);

    return;
}
#endif