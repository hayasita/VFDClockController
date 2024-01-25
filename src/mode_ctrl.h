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

// 操作モード
#define ctrlMode_VfdDisp        0   // VFD表示
#define ctrlMode_VfdCtrl        1   // VFD設定
#define ctrlMode_M5oled         2   // M5OLED設定
#define ctrlMode_Oled           3   // OLED設定

// キー入力
#define KEY_WIFI_S  0x01  // Hard SW0 Short ON 
#define KEY_SET_S   0x02  // Hard SW1 Short ON
#define KEY_UP_S    0x04  // Hard SW2 Short ON
#define KEY_DOWN_S  0x08  // Hard SW3 Short ON
#define KEY_WIFI_L  0x81  // Hard SW0 Long ON 
#define kEY_SET_L   0x82  // Hard SW1 Long ON
#define KEY_UP_L    0x84  // Hard SW2 Long ON
#define KEY_DOWN_L  0x88  // Hard SW3 Long ON

#define SWKEY_SET_S   0x02  // Soft SW1 Short ON
#define SWKEY_DISP_MODE_VFD_CLR 0x11  // VFD表示モードクリア要求

struct dispMode{
  uint8_t ctrlModeSelect;     // 操作モード選択　0:モード切替 1:設定操作
  uint8_t adjKeyData;         // 設定操作用キー情報
  uint8_t ctrlMode;           // 操作モード

  uint8_t dispModeVfd;        // VFD表示モード
  uint8_t dispModeVfdCtrl;    // VFD設定表示モード
  uint8_t dispModeM5OLED;     // M5OLED表示モード
  uint8_t dispModeOLED;       // OLED表示モード

};

class modeCtrl{
  public:
    modeCtrl(bool ssd1306,bool m5oled);     // コンストラクタ
    void modeIni(void);                     // モード初期化

    uint8_t getCtrlMode(void);              // 操作モード取得
    uint8_t getDispModeVfd(void);           // VFD表示モード取得
    uint8_t getDispModeVfdCtrl(void);       // VFD設定表示モード取得
    uint8_t getDispModeM5OLED(void);        // M5OLED表示モード取得
    uint8_t getDispModeOLED(void);          // OLED表示モード取得

    dispMode modeSet(uint8_t setKey,uint8_t swKey);   // 操作モード設定

  private:
    bool ssd1306Valid;          // OLED有無
    bool m5oledValid;           // M5OLED有無
    dispMode displayMode;       // モード情報

};


#undef GLOBAL
#endif