/**
 * @file mode_ctrl.cpp
 * @author hayasita04@gmail.com
 * @brief 動作モードの状態遷移の制御を行う。
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
//#include <M5Unified.h>
#include "mode_ctrl.h"

/**
 * @brief Construct a new mode Ctrl::mode Ctrl object
 * 
 * @param ssd1306 OLED装着
 * @param m5oled M5OLED装着
 */
modeCtrl::modeCtrl(bool ssd1306,bool m5oled)
{
  ssd1306Valid = ssd1306;     // OLED有無
  m5oledValid = m5oled;       // M5OLED有無

  vfdModeIni();               // VFD表示モードテーブル初期化
  vfdAdjModeIni();            // VFD設定表示モードテーブル初期化
  oledModeIni();              // OLED表示モードテーブル初期化
  m5OledModeIni();            // M5OLED表示モードテーブル初期化

  modeVFDIni();               // VFDモード初期化
  modeOledIni();              // OLED,M5OLEDモード初期化
/*
  Serial.println("modeCtrl::modeCtrl");
  Serial.print("ssd1306Valid:");
  Serial.println(ssd1306Valid);
  Serial.print("m5oled:");
  Serial.println(m5oled);
*/
  return;
}

/**
 * @brief モード初期化
 * 
 */
void modeCtrl::modeVFDIni(void)
{
  displayMode.ctrlModeSelect = 0;                 // 操作モード選択　0:モード切替
  displayMode.adjKeyData = 0;                     // 設定操作用キー情報初期化

  displayMode.ctrlMode = ctrlMode_VfdDisp;        // 操作モード初期化

  displayMode.dispModeVfdCount = 0;               // VFD表示モードテーブルカウンタ
  displayMode.dispModeVfd = dispModeVfdTbl[displayMode.dispModeVfdCount];         // VFD表示モード初期化

  return;
}

void modeCtrl::modeOledIni(void)                 // OLED,M5OLEDモード初期化
{
  displayMode.dispModeOLEDCount = 0;              // OLED表示モードテーブルカウンタ
  displayMode.dispModeM5OLED = dispModeM5oledTbl[displayMode.dispModeOLEDCount];  // OLED表示モード初期化

  displayMode.dispModeM5OLEDCount = 0;            // M5OLED表示モードテーブルカウンタ
  displayMode.dispModeOLED = dispModeOledTbl[displayMode.dispModeM5OLEDCount];    // M5OLED表示モード初期化

  return;
}


/**
 * @brief VFD表示モードテーブル初期化
 * 
 */
void modeCtrl::vfdModeIni(void)
{
  dispModeVfdTbl.push_back(VFD_DISP_DEFAULT);        // VFD表示　標準表示
  dispModeVfdTbl.push_back(VFD_DISP_TIMECLOCK);      // VFD表示　時刻表示
  dispModeVfdTbl.push_back(VFD_DISP_CALENDAR);       // VFD表示　カレンダー表示
  dispModeVfdTbl.push_back(VFD_DISP_TIME_SENSOR3);   // VFD表示　時刻・気温・湿度・気圧
  dispModeVfdTbl.push_back(VFD_DISP_TMP);            // VFD表示　気温表示

  return;
}

/**
 * @brief VFD設定表示モードテーブル初期化
 * 
 */
void modeCtrl::vfdAdjModeIni(void)
{
  dispModeVfdCtrTbl.push_back(VFD_DISP_CLOCK_ADJ);      // VFD設定表示 時計調整
  dispModeVfdCtrTbl.push_back(VFD_DISP_CAL_ADJ);        // カレンダー調整
  dispModeVfdCtrTbl.push_back(VFD_DISP_CLOCK_1224SEL);  // 12h24h表示切替
  dispModeVfdCtrTbl.push_back(VFD_DISP_FADETIME_ADJ);   // クロスフェード時間設定
  dispModeVfdCtrTbl.push_back(VFD_DISP_BRIGHTNESS_ADJ); // VFD輝度調整

  return;
}

/**
 * @brief OLED表示モードテーブル初期化
 * 
 */
void modeCtrl::oledModeIni(void)
{
  dispModeOledTbl.push_back(OLED_DISP_SENSOR_DATA);     // 環境センサデータ表示
  dispModeOledTbl.push_back(OLED_DISP_EVENTLOG_CTRL);   // EventLog操作情報
  return;
}

/**
 * @brief M5OLED表示モードテーブル初期化
 * 
 */
void modeCtrl::m5OledModeIni(void)
{
  dispModeM5oledTbl.push_back(M5OLED_DISP_SENSOR_DATA);   // 環境センサデータ表示
  dispModeM5oledTbl.push_back(M5OLED_DISP_EVENTLOG_CTRL); // EventLog操作情報
  return;
}

/**
 * @brief 操作モード取得
 * 
 * @return uint8_t 
 */
uint8_t modeCtrl::getCtrlMode(void)           // 操作モード取得
{
  return displayMode.ctrlMode;
}

/**
 * @brief VFD表示モード取得
 * 
 * @return uint8_t 
 */
uint8_t modeCtrl::getDispModeVfd(void)        // VFD表示モード取得
{
  return displayMode.dispModeVfd;
}

/**
 * @brief M5OLED表示モード取得
 * 
 * @return uint8_t 
 */
uint8_t modeCtrl::getDispModeM5OLED(void)     // M5OLED表示モード取得
{
  return displayMode.dispModeM5OLED;
}

/**
 * @brief OLED表示モード取得
 * 
 * @return uint8_t 
 */
uint8_t modeCtrl::getDispModeOLED(void)       // OLED表示モード取得
{
  return displayMode.dispModeOLED;
}

/**
 * @brief 操作モード設定
 * 
 * @param setKey 物理キー入力情報
 * @param swKey SW内部キー入力情報
 * @return dispMode 
 */
dispMode modeCtrl::modeSet(uint8_t setKey,uint8_t swKey)        // モード設定
{
  // 操作モード選択　0:モード切替
  if(displayMode.ctrlModeSelect == 0){
    displayMode.adjKeyData = 0;     // 設定操作用キー情報クリア
    if(displayMode.ctrlMode == ctrlMode_VfdDisp){   // 操作モード：VFD表示
      modeSetVFD(setKey,swKey);       // VFD表示モード設定
    }
    else if(displayMode.ctrlMode == ctrlMode_VfdCtrl){  // 操作モード：VFD設定
      modeSetVfdCnt(setKey,swKey);    // VFD設定表示モード設定
    }
    else if(displayMode.ctrlMode == ctrlMode_M5oled){   // 操作モード：M5OLED設定
      modeSetM5OLED(setKey,swKey);    // M5OLED表示モード設定
    }
    else if(displayMode.ctrlMode == ctrlMode_Oled){   // 操作モード：OLED設定
      modeSetOLED(setKey,swKey);      // OLED表示モード設定
    }
    else{

    }
  }
  else{
    displayMode.adjKeyData = setKey;     // 設定操作用キー情報設定
    // 操作モード選択　1:設定操作
     if(swKey == SWKEY_SET_S){   // 
//      else{
       displayMode.ctrlModeSelect = 0;                 // 操作モード選択　1:設定操作
//       Serial.println("操作モード選択へ移行");
//      }
      if(displayMode.dispModeVfd == VFD_DISP_CLOCK_1224SEL_SET){
        displayMode.dispModeVfd = VFD_DISP_CLOCK_1224SEL;
      }
    }

    // 設定処理強制終了
    if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
      modeVFDIni();                 // VFDモード初期化
//      Serial.println("操作モード選択へ強制移行");
    }
    else if(setKey == KEY_SET_S){
//      Serial.println(displayMode.adjKeyData);
    }
  }

  return displayMode;
}

/**
 * @brief VFD表示モード設定
 * 
 * @param setKey 物理キー入力情報
 * @param swKey SW内部キー入力情報
 */
void modeCtrl::modeSetVFD(uint8_t setKey,uint8_t swKey)
{
  if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
    displayMode.ctrlMode = ctrlMode_VfdCtrl;    // 操作モード：VFD表示 -> VFD設定
    displayMode.dispModeVfdCount = 0;
    displayMode.dispModeVfd = dispModeVfdCtrTbl[displayMode.dispModeVfdCount];  // VFD設定表示モード設定
  }
  else if(setKey == KEY_SET_S){   // SETKey SW1 Short ON
    displayMode.adjKeyData = setKey;     // 設定操作用キー情報設定;
  }
  else if((setKey == KEY_UP_S) || (setKey == KEY_DOWN_S)){    // ▲Key SW2 Short ON or ▼Key SW3 Short ON
    displayMode.dispModeVfd = updownKeyModeSet(setKey,dispModeVfdTbl,&displayMode.dispModeVfdCount);
  }

  if(swKey == SWKEY_DISP_MODE_VFD_CLR){
    displayMode.dispModeVfdCount = 0;
    displayMode.dispModeVfd = dispModeVfdTbl[displayMode.dispModeVfdCount];  // VFD表示モード初期化
  }

  return;
}

/**
 * @brief VFD設定表示モード設定
 * 
 * @param setKey 物理キー入力情報
 * @param swKey SW内部キー入力情報
 */
void modeCtrl::modeSetVfdCnt(uint8_t setKey,uint8_t swKey)
{
  if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
    if(ssd1306Valid){
      displayMode.ctrlMode = ctrlMode_Oled;     // M5OLEDなし　OLEDあり 操作モード：VFD設定 -> OLED設定
      displayMode.dispModeVfdCount = 0;
      displayMode.dispModeVfd = dispModeVfdTbl[displayMode.dispModeVfdCount];  // VFD表示モード初期化
    }
    else if(m5oledValid){
      displayMode.ctrlMode = ctrlMode_M5oled;   // M5OLEDあり　操作モード：VFD設定 -> M5OLED設定
      displayMode.dispModeVfdCount = 0;
      displayMode.dispModeVfd = dispModeVfdTbl[displayMode.dispModeVfdCount];  // VFD表示モード初期化
    }
    else{
      displayMode.ctrlMode = ctrlMode_VfdDisp;  // M5OLEDなし　OLEDなし　操作モード：VFD設定 -> VFD表示
    }
  }
  else if(setKey == KEY_SET_S){
    displayMode.ctrlModeSelect = 1;                 // 操作モード選択　1:設定操作
//        Serial.println("操作設定へ移行");
      if(displayMode.dispModeVfd == VFD_DISP_CLOCK_1224SEL){
        displayMode.dispModeVfd = VFD_DISP_CLOCK_1224SEL_SET;
      }
      else if(displayMode.dispModeVfd == VFD_DISP_CLOCK_1224SEL_SET){
        displayMode.dispModeVfd = VFD_DISP_CLOCK_1224SEL;
      }

  }
  else if((setKey == KEY_UP_S) || (setKey == KEY_DOWN_S)){    // ▲Key SW2 Short ON or ▼Key SW3 Short ON
    displayMode.dispModeVfd = updownKeyModeSet(setKey,dispModeVfdCtrTbl,&displayMode.dispModeVfdCount);
  }

  return;
}

/**
 * @brief OLED表示モード設定
 * 
 * @param setKey  物理キー入力情報
 * @param swKey   SW内部キー入力情報
 */
void modeCtrl::modeSetOLED(uint8_t setKey,uint8_t swKey)
{
  if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
    if(m5oledValid){
      displayMode.ctrlMode = ctrlMode_M5oled;   // M5OLEDあり　操作モード：VFD設定 -> M5OLED設定
    }
    else{
      displayMode.ctrlMode = ctrlMode_VfdDisp;    // 操作モード：OLED設定 -> VFD表示
    }
  }
  else if((setKey == KEY_UP_S) || (setKey == KEY_DOWN_S)){    // ▲Key SW2 Short ON or ▼Key SW3 Short ON
    displayMode.dispModeOLED = updownKeyModeSet(setKey,dispModeOledTbl,&displayMode.dispModeOLEDCount);
  }

  return;
}

/**
 * @brief M5OLED表示モード設定
 * 
 * @param setKey  物理キー入力情報
 * @param swKey   SW内部キー入力情報
 */
void modeCtrl::modeSetM5OLED(uint8_t setKey,uint8_t swKey)
{
  if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
    displayMode.ctrlMode = ctrlMode_VfdDisp;  // OLEDなし　操作モード：M5OLED設定 -> VFD表示
  }
  else if((setKey == KEY_UP_S) || (setKey == KEY_DOWN_S)){    // ▲Key SW2 Short ON or ▼Key SW3 Short ON
    displayMode.dispModeM5OLED = updownKeyModeSet(setKey,dispModeM5oledTbl,&displayMode.dispModeM5OLEDCount);
  }

  return;
}

/**
 * @brief UpDownKeyによる操作モードテーブル参照
 * 
 * @param setKey    物理キー入力情報
 * @param modeTbl   表示モードテーブル
 * @param modeCount 表示モードテーブルポインタ
 * @return uint8_t  表示モード
 */
uint8_t modeCtrl::updownKeyModeSet(uint8_t setKey,std::vector<uint8_t> modeTbl,uint8_t *modeCount)
{
  if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
    if((*modeCount+1) < modeTbl.size()){
      (*modeCount)++;
    }
    else{
      *modeCount = 0;
    }
  }
  else if(setKey == KEY_DOWN_S){  // ▼Key SW3 Short ON
    if(*modeCount > 0){
      (*modeCount)--;
    }
    else{
      *modeCount = modeTbl.size() -1;
    }
  }

  return modeTbl[*modeCount];  // 表示モード
}
