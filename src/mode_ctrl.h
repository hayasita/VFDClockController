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
#include <vector>

// 操作モード
#define ctrlMode_VfdDisp        0   // VFD表示
#define ctrlMode_VfdCtrl        1   // VFD設定
#define ctrlMode_Oled           2   // OLED設定
#define ctrlMode_M5oled         3   // M5OLED設定

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

// VFD表示モード
#define VFD_DISP_DEFAULT                0     // VFD表示　標準表示
#define VFD_DISP_TIMECLOCK              1     // VFD表示　時刻表示
#define VFD_DISP_CALENDAR               2     // VFD表示　カレンダー表示
#define VFD_DISP_TIME_SENSOR3           3     // VFD表示　時刻・気温・湿度・気圧表示
#define VFD_DISP_TMP                    4     // VFD表示　気温表示

#define VFD_DISP_NUMBER                 9     // VFD表示番号

// VFD設定表示モード
#define VFD_DISP_CLOCK_ADJ              10    // 時計調整
#define VFD_DISP_CLOCK_ADJ_SET          11    // 時計調整実行
#define VFD_DISP_CAL_ADJ                12    // カレンダー調整
#define VFD_DISP_CAL_ADJ_SET            13    // カレンダー調整実行
#define VFD_DISP_CLOCK_1224SEL          14    // 12h24h表示切替
#define VFD_DISP_CLOCK_1224SEL_SET      15    // 12h24h表示切替実行
#define VFD_DISP_FADETIME_ADJ           16    // クロスフェード時間設定
#define VFD_DISP_FADETIME_ADJ_SET       17    // クロスフェード時間設定実行
#define VFD_DISP_BRIGHTNESS_ADJ         18    // VFD輝度調整
#define VFD_DISP_BRIGHTNESS_ADJ_SET     19    // VFD輝度調整実行
#define VFD_DISP_BRIGHTNESS_VIEW        20    // VFD輝度設定値表示

// OLED表示モード
#define OLED_DISP_SENSOR_DATA           1     // 環境センサデータ表示
#define OLED_DISP_EVENTLOG_CTRL         2     // EventLog操作情報

// M5OLED表示モード
#define M5OLED_DISP_SENSOR_DATA         1     // 環境センサデータ表示
#define M5OLED_DISP_EVENTLOG_CTRL       2     // EventLog操作情報

struct dispMode{
  uint8_t ctrlModeSelect;       // 操作モード選択　0:モード切替 1:設定操作
  uint8_t adjKeyData;           // 設定操作用キー情報

  uint8_t ctrlMode;             // 操作モード

  uint8_t dispModeVfd;          // VFD表示モード
  uint8_t dispModeVfdCount;     // VFD表示モードテーブルカウンタ

  uint8_t dispModeOLED;         // OLED表示モード
  uint8_t dispModeOLEDCount;    // OLED表示モードテーブルカウンタ

  uint8_t dispModeM5OLED;       // M5OLED表示モード
  uint8_t dispModeM5OLEDCount;  // M5OLED表示モードテーブルカウンタ

};

class modeCtrl{
  public:
    modeCtrl(bool ssd1306,bool m5oled);     // コンストラクタ
    void modeIni(void);                     // モード初期化

    uint8_t getCtrlMode(void);              // 操作モード取得
    uint8_t getDispModeVfd(void);           // VFD表示モード取得
    uint8_t getDispModeM5OLED(void);        // M5OLED表示モード取得
    uint8_t getDispModeOLED(void);          // OLED表示モード取得

    dispMode modeSet(uint8_t setKey,uint8_t swKey);   // 操作モード設定
    void modeSetVFD(uint8_t setKey,uint8_t swKey);    // VFD表示モード設定
    void modeSetVfdCnt(uint8_t setKey,uint8_t swKey); // VFD設定表示モード設定
    void modeSetM5OLED(uint8_t setKey,uint8_t swKey); // M5OLED表示モード設定
    void modeSetOLED(uint8_t setKey,uint8_t swKey);   // OLED表示モード設定

  private:
    bool ssd1306Valid;          // OLED有無
    bool m5oledValid;           // M5OLED有無
    dispMode displayMode;       // モード情報

    void vfdModeIni(void);                      // VFD表示モードテーブル初期化
    void vfdAdjModeIni(void);                   // VFD設定表示モードテーブル初期化
    void oledModeIni(void);                     // OLED表示モードテーブル初期化
    void m5OledModeIni(void);                   // M5OLED表示モードテーブル初期化

    std::vector<uint8_t> dispModeVfdTbl;        // VFD表示モードテーブル
    std::vector<uint8_t> dispModeVfdCtrTbl;     // VFD設定表示モードテーブル
    std::vector<uint8_t> dispModeOledTbl;       // OLRD表示モードテーブル
    std::vector<uint8_t> dispModeM5oledTbl;     // M5OLRD表示モードテーブル

    uint8_t updownKeyModeSet(uint8_t setKey,std::vector<uint8_t> modeTbl,uint8_t *modeCount);   // UpDownKeyによる操作モードテーブル参照

};


#undef GLOBAL
#endif