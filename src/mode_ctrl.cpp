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
  ssd1306Valid = ssd1306;
  m5oledValid = m5oled;

  displayMode.ctrlModeSelect = 0;                 // 操作モード選択　0:モード切替
  displayMode.dispModeVfd = dispModeVfd_Default;        // VFD表示モード初期化
  displayMode.dispModeVfdCtrl = 0;               //VFD設定表示モード
  displayMode.dispModeM5OLED = dispModeOled_Default;    // M5OLED表示モード初期化
  displayMode.dispModeOLED = dispModeOled_Default;      // OLED表示モード初期化

//  dispCtrlTrg = dispCtrlTrg_Vfd;            // ディスプレイ操作ターゲット初期化
  displayMode.ctrlMode = ctrlMode_VfdDisp;              // 操作モード初期化
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
 * @brief VFD設定表示モード取得
 * 
 * @return uint8_t 
 */
uint8_t modeCtrl::getDispModeVfdCtrl(void)       // VFD設定表示モード取得
{
  return displayMode.dispModeVfdCtrl;
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
      if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
        displayMode.ctrlMode = ctrlMode_VfdCtrl;    // 操作モード：VFD表示 -> VFD設定
      }
      else if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
        displayMode.dispModeVfd++;
      }
      else if(setKey == KEY_DOWN_S){  // ▼Key SW3 Short ON
        displayMode.dispModeVfd--;
      }
      else if(setKey == KEY_SET_S){   // SETKey SW1 Short ON
        displayMode.adjKeyData = setKey;     // 設定操作用キー情報設定;
      }

      if(swKey == SWKEY_DISP_MODE_VFD_CLR){
        displayMode.dispModeVfd = dispModeVfd_Default;        // VFD表示モード初期化
      }

    }
    else if(displayMode.ctrlMode == ctrlMode_VfdCtrl){  // 操作モード：VFD設定
      if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
        if(m5oledValid){
          displayMode.ctrlMode = ctrlMode_M5oled;   // M5OLEDあり　操作モード：VFD設定 -> M5OLED設定
        }
        else if(ssd1306Valid){
          displayMode.ctrlMode = ctrlMode_Oled;     // M5OLEDなし　OLEDあり 操作モード：VFD設定 -> OLED設定
        }
        else{
          displayMode.ctrlMode = ctrlMode_VfdDisp;  // M5OLEDなし　OLEDなし　操作モード：VFD設定 -> VFD表示
        }
      }
      else if(setKey == KEY_SET_S){
        displayMode.ctrlModeSelect = 1;                 // 操作モード選択　1:設定操作
//        Serial.println("操作設定へ移行");
      }
      else if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
        displayMode.dispModeVfdCtrl++;
      }
      else if(setKey == KEY_DOWN_S){  // ▼Key SW3 Short ON
        displayMode.dispModeVfdCtrl--;
      }

    }
    else if(displayMode.ctrlMode == ctrlMode_M5oled){   // 操作モード：M5OLED設定
      if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
        if(ssd1306Valid){
          displayMode.ctrlMode = ctrlMode_Oled;     // OLEDあり　操作モード：M5OLED設定 -> OLED設定
        }
        else{
          displayMode.ctrlMode = ctrlMode_VfdDisp;  // OLEDなし　操作モード：M5OLED設定 -> VFD表示
        }
      }
      else if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
        if(displayMode.dispModeM5OLED == dispModeOled_SensorData){
          displayMode.dispModeM5OLED = dispModeOled_EventLogCtrl;         // M5OLED表示モード：環境センサデータ表示 -> EventLog操作情報
        }
        else if(displayMode.dispModeM5OLED == dispModeOled_EventLogCtrl){
          displayMode.dispModeM5OLED = dispModeOled_SensorData;           // M5OLED表示モード：EventLog操作情報 -> 環境センサデータ表示
        }
      }
      else if(setKey == KEY_DOWN_S){  // ▼Key SW3 Short ON
        if(displayMode.dispModeM5OLED == dispModeOled_SensorData){
          displayMode.dispModeM5OLED = dispModeOled_EventLogCtrl;         // M5OLED表示モード：環境センサデータ表示 -> EventLog操作情報
        }
        else if(displayMode.dispModeM5OLED == dispModeOled_EventLogCtrl){
          displayMode.dispModeM5OLED = dispModeOled_SensorData;           // M5OLED表示モード：EventLog操作情報 -> 環境センサデータ表示
        }
      }

    }
    else if(displayMode.ctrlMode == ctrlMode_Oled){   // 操作モード：OLED設定
      if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
        displayMode.ctrlMode = ctrlMode_VfdDisp;    // 操作モード：OLED設定 -> VFD表示
      }

      else if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
        if(displayMode.dispModeOLED == dispModeOled_SensorData){
          displayMode.dispModeOLED = dispModeOled_EventLogCtrl;         // OLED表示モード：環境センサデータ表示 -> EventLog操作情報
        }
        else if(displayMode.dispModeOLED == dispModeOled_EventLogCtrl){
          displayMode.dispModeOLED = dispModeOled_SensorData;           // OLED表示モード：EventLog操作情報 -> 環境センサデータ表示
        }
      }
      else if(setKey == KEY_DOWN_S){  // ▼Key SW2 Short ON
        if(displayMode.dispModeOLED == dispModeOled_SensorData){
          displayMode.dispModeOLED = dispModeOled_EventLogCtrl;         // OLED表示モード：環境センサデータ表示 -> EventLog操作情報
        }
        else if(displayMode.dispModeOLED == dispModeOled_EventLogCtrl){
          displayMode.dispModeOLED = dispModeOled_SensorData;           // OLED表示モード：EventLog操作情報 -> 環境センサデータ表示
        }
      }

    }
    else{

    }
  }
  else{
    displayMode.adjKeyData = setKey;     // 設定操作用キー情報設定
    // 操作モード選択　1:設定操作
     if(swKey == SWKEY_SET_S){   // 
       displayMode.ctrlModeSelect = 0;                 // 操作モード選択　1:設定操作
//       Serial.println("操作モード選択へ移行");
    }

    // 設定処理強制終了
    if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
      displayMode.ctrlModeSelect = 0;                 // 操作モード選択　0:モード切替
      displayMode.ctrlMode = ctrlMode_VfdDisp;
      displayMode.adjKeyData = 0;     // 設定操作用キー情報クリア
//      Serial.println("操作モード選択へ強制移行");

    }
    else if(setKey == KEY_SET_S){
//      Serial.println(displayMode.adjKeyData);
    }
  }

  return displayMode;
}
/*
void DispCtr::dispModeSet(uint8_t setKey)
{
  String status;

  status = "";
  if(dispMode == MODE_STD_DISP){
    if(setKey == kEY_SET_L){
      dispMode = MODE_CLOCK_ADJ;
      status = "Mode : MODE_CLOCK_ADJ";
    }

    if(setKey == KEY_UP_S){
      stdDispFormat++;
    }
    else if(setKey == KEY_DOWN_S){
      stdDispFormat--;
    }

  }
  else if((dispMode >= MODE_CLOCK_ADJ) && (dispMode < MODE_ERR_)){
    if(setKey == kEY_SET_L){
      dispMode = MODE_STD_DISP;
      status = "Mode : MODE_STD_DISP";
    }
  }

  if(dispMode != lastDispMode){     // モード変更あり
    lastDispMode = dispMode;        // 前回モード = 今回モード
    dispScrolldatMakeIni();         // スクロール表示データ初期化
//    dispBlinkingMakeIni();          // 表示データ点滅初期化
  }

  if(status.length() != 0){
    Serial.println(status);
  }
  return;
}
*/