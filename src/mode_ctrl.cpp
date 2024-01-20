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

  dispModeVfd = dispModeVfd_Default;        // VFD表示モード初期化
  dispModeM5OLED = dispModeOled_Default;    // M5OLED表示モード初期化
  dispModeOLED = dispModeOled_Default;      // OLED表示モード初期化

//  dispCtrlTrg = dispCtrlTrg_Vfd;            // ディスプレイ操作ターゲット初期化
  ctrlMode = ctrlMode_VfdDisp;              // 操作モード初期化
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
  return ctrlMode;
}

/**
 * @brief VFD表示モード取得
 * 
 * @return uint8_t 
 */
uint8_t modeCtrl::getDispModeVfd(void)        // VFD表示モード取得
{
  return dispModeVfd;
}

/**
 * @brief M5OLED表示モード取得
 * 
 * @return uint8_t 
 */
uint8_t modeCtrl::getDispModeM5OLED(void)     // M5OLED表示モード取得
{
  return dispModeM5OLED;
}

/**
 * @brief OLED表示モード取得
 * 
 * @return uint8_t 
 */
uint8_t modeCtrl::getDispModeOLED(void)       // OLED表示モード取得
{
  return dispModeOLED;
}

/**
 * @brief 操作モード設定
 * 
 * @param setKey 
 */
//void modeCtrl::modeSet(uint8_t setKey)        // モード設定
dispMode modeCtrl::modeSet(uint8_t setKey)        // モード設定
{
  if(ctrlMode == ctrlMode_VfdDisp){   // 操作モード：VFD表示
    if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
      ctrlMode = ctrlMode_VfdCtrl;    // 操作モード：VFD表示 -> VFD設定
    }
    else if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
    }
    else if(setKey == KEY_DOWN_S){  // ▼Key SW3 Short ON
    }

  }
  else if(ctrlMode == ctrlMode_VfdCtrl){  // 操作モード：VFD設定
    if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
      if(m5oledValid){
        ctrlMode = ctrlMode_M5oled;   // M5OLEDあり　操作モード：VFD設定 -> M5OLED設定
      }
      else if(ssd1306Valid){
        ctrlMode = ctrlMode_Oled;     // M5OLEDなし　OLEDあり 操作モード：VFD設定 -> OLED設定
      }
      else{
        ctrlMode = ctrlMode_VfdDisp;  // M5OLEDなし　OLEDなし　操作モード：VFD設定 -> VFD表示
      }
    }
    else if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
    }
    else if(setKey == KEY_DOWN_S){  // ▼Key SW3 Short ON
    }

  }
  else if(ctrlMode == ctrlMode_M5oled){   // 操作モード：M5OLED設定
    if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
      if(ssd1306Valid){
        ctrlMode = ctrlMode_Oled;     // OLEDあり　操作モード：M5OLED設定 -> OLED設定
      }
      else{
        ctrlMode = ctrlMode_VfdDisp;  // OLEDなし　操作モード：M5OLED設定 -> VFD表示
      }
    }
    else if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
      if(dispModeM5OLED == dispModeOled_SensorData){
        dispModeM5OLED = dispModeOled_EventLogCtrl;         // M5OLED表示モード：環境センサデータ表示 -> EventLog操作情報
      }
      else if(dispModeM5OLED == dispModeOled_EventLogCtrl){
        dispModeM5OLED = dispModeOled_SensorData;           // M5OLED表示モード：EventLog操作情報 -> 環境センサデータ表示
      }
    }
    else if(setKey == KEY_DOWN_S){  // ▼Key SW3 Short ON
      if(dispModeM5OLED == dispModeOled_SensorData){
        dispModeM5OLED = dispModeOled_EventLogCtrl;         // M5OLED表示モード：環境センサデータ表示 -> EventLog操作情報
      }
      else if(dispModeM5OLED == dispModeOled_EventLogCtrl){
        dispModeM5OLED = dispModeOled_SensorData;           // M5OLED表示モード：EventLog操作情報 -> 環境センサデータ表示
      }
    }

  }
  else if(ctrlMode == ctrlMode_Oled){   // 操作モード：OLED設定
    if(setKey == kEY_SET_L){        // SETKey SW1 Long ON
      ctrlMode = ctrlMode_VfdDisp;    // 操作モード：OLED設定 -> VFD表示
    }

    else if(setKey == KEY_UP_S){    // ▲Key SW2 Short ON
      if(dispModeOLED == dispModeOled_SensorData){
        dispModeOLED = dispModeOled_EventLogCtrl;         // OLED表示モード：環境センサデータ表示 -> EventLog操作情報
      }
      else if(dispModeOLED == dispModeOled_EventLogCtrl){
        dispModeOLED = dispModeOled_SensorData;           // OLED表示モード：EventLog操作情報 -> 環境センサデータ表示
      }
    }
    else if(setKey == KEY_DOWN_S){  // ▼Key SW2 Short ON
      if(dispModeOLED == dispModeOled_SensorData){
        dispModeOLED = dispModeOled_EventLogCtrl;         // OLED表示モード：環境センサデータ表示 -> EventLog操作情報
      }
      else if(dispModeOLED == dispModeOled_EventLogCtrl){
        dispModeOLED = dispModeOled_SensorData;           // OLED表示モード：EventLog操作情報 -> 環境センサデータ表示
      }
    }

  }
  else{

  }

  dispMode ret;
  ret.ctrlMode = ctrlMode;           // 操作モード
  ret.dispModeVfd = dispModeVfd;        // VFD表示モード
  ret.dispModeOLED = dispModeOLED;       // OLED表示モード
  ret.dispModeM5OLED = dispModeM5OLED;     // M5OLED表示モード

  return ret;
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