/**
 * @file mode_ctrl.h
 * @author hayasita04@gmail.com
 * @brief 動作モードの状態遷移の制御を行う。
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef mode_ctrl_h
#define mode_ctrl_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <cstdint>

#define dispModeVdf_Std         0

#define dispModeVfd_Default     dispModeVdf_Std

#define dispModeOled_SensorData     1   // 環境センサデータ表示
#define dispModeOled_EventLogCtrl   2   // EventLog操作情報

#define dispModeOled_Default    dispModeOled_SensorData

/*
#define dispCtrlTrg_Vfd         0
#define dispCtrlTrg_M5OLED      1
#define dispCtrlTrg_M5OLED      2
*/

// 操作モード
#define ctrlMode_VfdDisp        0   // VFD表示
#define ctrlMode_VfdCtrl        1   // VFD設定
#define ctrlMode_M5oled         2   // M5OLED設定
#define ctrlMode_Oled           3   // OLED設定

// キー入力
#define KEY_WIFI_L  0x81  // SW0 Long ON 
#define kEY_SET_L   0x82  // SW1 Long ON
#define KEY_SET_S   0x01  // SW1 Short ON
#define KEY_UP_S    0x04  // SW2 Short ON
#define KEY_DOWN_S  0x08  // SW3 Short ON


class modeCtrl{
  public:
    modeCtrl(bool ssd1306,bool m5oled);     // コンストラクタ

    uint8_t getCtrlMode(void);              // 操作モード取得
    uint8_t getDispModeVfd(void);           // VFD表示モード取得
    uint8_t getDispModeM5OLED(void);        // M5OLED表示モード取得
    uint8_t getDispModeOLED(void);          // OLED表示モード取得

    void modeSet(uint8_t setKey);           // 操作モード設定

  private:
    bool ssd1306Valid;          // OLED有無
    bool m5oledValid;           // M5OLED有無

    uint8_t ctrlMode;           // 操作モード

    uint8_t dispModeVfd;        // VFD表示モード
    uint8_t dispModeM5OLED;     // M5OLED表示モード
    uint8_t dispModeOLED;       // OLED表示モード

//    uint8_t dispCtrlTrg;        // ディスプレイ操作ターゲット


};


#undef GLOBAL
#endif