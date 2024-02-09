#ifndef vfd_disp_h
#define vfd_disp_h

#include "mode_ctrl.h"

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <M5Unified.h>

const unsigned long keta_dat[] = {
  0x040000,    // 1
  0x080000,    // 2
  0x000001,    // 3
  0x008000,    // 4
  0x400000,    // 5
  0x000100,    // 6
  0x004000,    // 7
  0x002000,    // 8
  0x000800     // 9
};
#define ELE_A  0x010000
#define ELE_B  0x020000
#define ELE_C  0x100000
#define ELE_D  0x800000
#define ELE_E  0x000200
#define ELE_F  0x000400
#define ELE_G  0x001000
#define ELE_H  0x200000
// 表示データ定義
#define DISP_00  0
#define DISP_01  1
#define DISP_02  2
#define DISP_03  3
#define DISP_04  4
#define DISP_05  5
#define DISP_06  6
#define DISP_07  7
#define DISP_08  8
#define DISP_09  9
#define DISP_K0  10
#define DISP_K1  11
#define DISP_K2  12
#define DISP_K3  13
#define DISP_NON  13
#define DISP_A    14
#define DISP_B    15
#define DISP_C    16
#define DISP_D    17
#define DISP_E    18
#define DISP_F    19
#define DISP_G    20
#define DISP_H    21
#define DISP_I    22
#define DISP_J    23
#define DISP_K    24
#define DISP_L    25
#define DISP_M    26
#define DISP_N    27
#define DISP_O    28
#define DISP_P    29
#define DISP_Q    30
#define DISP_R    31
#define DISP_S    32
#define DISP_T    33
#define DISP_U    34
#define DISP_V    35
#define DISP_W    36
#define DISP_X    37
#define DISP_Y    38
#define DISP_Z    39

// 表示フォント定義
const unsigned long font[] = {
  ELE_A | ELE_B | ELE_C | ELE_D | ELE_E | ELE_F,            // 0
  ELE_B | ELE_C,                                            // 1
  ELE_A | ELE_B | ELE_G | ELE_E | ELE_D,                    // 2
  ELE_A | ELE_B | ELE_C | ELE_D | ELE_G,                    // 3
  ELE_B | ELE_C | ELE_F | ELE_G,                            // 4
  ELE_A | ELE_F | ELE_G | ELE_C | ELE_D,                    // 5
  ELE_A | ELE_C | ELE_D | ELE_E | ELE_F | ELE_G,            // 6
  ELE_A | ELE_B | ELE_C | ELE_F,                            // 7
  ELE_A | ELE_B | ELE_C | ELE_D | ELE_E | ELE_F | ELE_G,    // 8
  ELE_A | ELE_B | ELE_C | ELE_D | ELE_F | ELE_G,            // 9
  ELE_F,                                                    // ●
  ELE_G,                                                    // －
  ELE_F | ELE_G,                                            // ●－
  0x000000,                                                 // NULL
  ELE_A | ELE_B | ELE_C | ELE_E | ELE_F | ELE_G,            // A
  ELE_C | ELE_D | ELE_E | ELE_F | ELE_G,                    // B
  ELE_D | ELE_E | ELE_G,                                    // C
  ELE_B | ELE_C | ELE_D | ELE_E | ELE_G,                    // D
  ELE_A | ELE_D | ELE_E | ELE_F | ELE_G,                    // E
  ELE_A | ELE_E | ELE_F | ELE_G,                            // F
  ELE_A | ELE_C | ELE_D | ELE_E | ELE_F,                    // G
  ELE_C | ELE_E | ELE_F | ELE_G,                            // H
  ELE_E | ELE_F,                                            // I
  ELE_B | ELE_C | ELE_D | ELE_E,                            // J
  ELE_D | ELE_E | ELE_F | ELE_G,                            // K
  ELE_D | ELE_E | ELE_F,                                    // L
  ELE_A | ELE_B | ELE_C | ELE_E | ELE_F,                    // M
  ELE_C | ELE_E | ELE_G,                                    // N
  ELE_C | ELE_D | ELE_E | ELE_G,                            // O
  ELE_A | ELE_B | ELE_E | ELE_F | ELE_G,                    // P
  ELE_A | ELE_B | ELE_C | ELE_F | ELE_G,                    // Q
  ELE_E | ELE_G,                                            // R
  ELE_C | ELE_D | ELE_F | ELE_G,                            // S
  ELE_A | ELE_E | ELE_F,                                    // T
  ELE_C | ELE_D | ELE_E,                                    // U
  ELE_B | ELE_C | ELE_D | ELE_E | ELE_F,                    // V
  ELE_A | ELE_D | ELE_G,                                    // W
  ELE_B | ELE_C | ELE_E | ELE_F | ELE_G,                    // X
  ELE_B | ELE_C | ELE_D | ELE_F | ELE_G,                    // Y
  ELE_A | ELE_B | ELE_D | ELE_E,                            // Z
};
#define FONT_MAX (sizeof(font) / sizeof(unsigned long))

#define MODE_STD_DISP 0   // 標準表示
#define MODE_ADJ_DISP 1   // 設定表示

#define MODE_CLOCK_ADJ                  10    // 時計調整
#define MODE_CLOCK_ADJ_SET              11    // 時計調整実行

#define MODE_ERR_                       100   // エラー番号閾値　100以上はエラーモード定義として使用する。

#define DISP_KETAMAX 9                          // VFD表示桁数
GLOBAL unsigned long disp[DISP_KETAMAX];        // 数値表示データ
GLOBAL unsigned long disp_last[DISP_KETAMAX];   // クロスフェード用前回数値表示データ
GLOBAL unsigned char disp_p[DISP_KETAMAX];      // 各桁ピリオドデータ

GLOBAL uint8_t *brp;                            // 輝度情報ポインタ
GLOBAL uint8_t brightness_dig[DISP_KETAMAX];    // 表示各桁輝度
GLOBAL uint8_t disp_fadecount[DISP_KETAMAX];    // クロスフェードサイクルカウンタ
GLOBAL uint16_t disp_fadetimei;                 // クロスフェード時間（割込）(msec)
//GLOBAL portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
GLOBAL portMUX_TYPE timerMux;

struct DISPLAY_DATA{
  uint16_t temp;
  uint16_t humi;
  uint16_t pressure;
  struct tm timeInfo;
};

/**
 * @brief 表示データ作成処理テーブル
 * 
 */
class dispTbl{
  public:
    uint8_t dispModeVfd;                      // VFD表示モード
    uint8_t dispNumSq;                        // VFD表示フォーマット表示有無　0:無 1:有
    std::function<void()> dispDatMakeFunc;    // 表示データ作成メンバ
    std::function<uint8_t()> dispAdjFunc;     // 設定処理メンバ
};

class dispDatMakeFunc{
  public:
    dispDatMakeFunc(void);
    uint8_t dispModeSet(dispMode);                  // 表示モード設定
    void vfdDispDataMakeSel(struct DISPLAY_DATA inputData);          // VFD表示データ作製処理選択
    void dispTableIni(void);                      // 表示データ作成処理テーブル初期化

    uint16_t *dispTmp;                      // 各桁表示データ
    uint8_t *piriodTmp;                     // 各桁ピリオド
    uint8_t vfdDispFormat;                  // VFD表示フォーマット指定
    uint8_t lastVfdDispFormat;              // 前回VFD表示フォーマット指定

  private:
    // 表示データ作成処理実行テーブル処理
    std::vector<dispTbl> dispTableArray;          // 表示データ作成処理テーブル
    struct DISPLAY_DATA dispInputData;            // 表示用データ
    void dispDataMakeExec(uint8_t index);         // 表示データ作成処理実行
    
    // 表示データスクロール処理
    unsigned long scroll_tim_nowl;
    uint8_t disp_point;
    void dispScrolldatMakeIni(void);                // スクロール表示データ初期化
    void dispScrolldatMake(const char *disp_data,uint8_t startp,uint8_t dispnum);

    // 表示データ点滅処理
    uint8_t blinkingState;
    uint8_t blinkingSqf;
    long blinkingTimNowl;
    void dispBlinkingMakeIni(void);                 // 表示データ点滅初期化
    void dispBlinkingMake(uint8_t startp,uint8_t dispnum,uint8_t mode,long blink_interval);

    // 各表示モードの表示データ作成処理
    void dispNumber(void);                          // VFD表示番号表示データ作成
    void dispClock(struct tm timeInfo);             // 時刻表示データ作成
    void dispCalender(struct tm timeInfo);          // 日付表示データ作成
    void dispTemp(struct DISPLAY_DATA inputData);   // 温度表示データ作成
    void dispHum(struct DISPLAY_DATA inputData);    // 湿度表示データ作成
    void dispPres(struct DISPLAY_DATA inputData);   // 気圧表示データ作成
    void dispLoop1(struct DISPLAY_DATA inputData);  // 時刻＋温度＋湿度＋気圧

    void clockAdjtitleDispdatMake(void);            // 時刻設定タイトル表示
    void clockAdjDispdatMake(void);                 // 時刻設定画面表示
    void calenderAdjtitleDispdatMake(void);         // カレンダー設定タイトル表示
    void calenderAdjDispdatMake(void);              // カレンダー調整実行
    void clock1224setAdjtitleDispdatMake(void);     // 12h24h表示切替
    void clock1224setDispdatMake(void);             // 12h24h表示切替実行
    void crossfadeAdjTitleDispdatMake(void);        // クロスフェード時間設定
    void crossfadeAdjDispdatMake(void);             // クロスフェード時間設定実行
    void brightnessAdjtitleDispdatMake(void);       // VFD輝度調整
    void brightnessAdjDispdatMake(void);            // VFD輝度調整実行
    void brightnessDataViewDispdatMake(void);       // VFD輝度設定値表示

    // 各設定モードの実行処理
    uint8_t dummyExec(void);                        // 処理なし
    uint8_t dispDefaultSetExec(void);               // 標準表示設定処理
    uint8_t clock1224setAdjExec(void);              // 12h24h表示切替処理

    uint8_t vfdDispNum;                     // VFD表示フォーマット表示番号
    uint8_t adjKeyData;                     // 設定操作用キー情報

};

class DispCtr : public dispDatMakeFunc
{
  public:
    DispCtr(void);
    void init(void);
    void dataMake(struct DISPLAY_DATA inputData);   // 表示データ作成
    void dispLCD(struct tm timeInfo);

  private:
    void brightness_ini(void);
    uint16_t *dispdataTmp;                  // 各桁表示データ(font情報)
    uint16_t dispdata;                      // 表示データ作成用tmp
    uint16_t fadetime_tmpw;                 // クロスフェード時間受け渡し用データ
    uint8_t fade;                           // クロスフェードON/OFF

    // 
    void timeDispLcd(struct tm timeInfo);
    void debugTimeConf(void);
    void debugDispConf(void);

};

// 時刻表示フォーマット
#define timeDispFormHhmmss    1   // TimeDisplay Format hh.mm.ss
#define timeDispFormHmmss     2   // TimeDisplay Format h.mm.ss
#define timeDispFormMmsshh    3   // TimeDisplay Format mm.ss.hh

// 日付表示フォーマット
#define dateDispFormYyyymmdd  1   // DateDisplay Format yyyy.mm.dd
#define dateDispFormYymmdd    2   // DateDisplay Format yy.mm.dd
#define dateDispFormMmddyyyy  3   // DateDisplay Format mm.dd.yyyy
#define dateDispFormMmddyy    4   // DateDisplay Format mm.dd.yy
#define dateDispFormDdmmyyyy  5   // DateDisplay Format dd.mm.yyyy
#define dateDispFormDdmmyy    6   // DateDisplay Format dd.mm.yy

#undef GLOBAL
#endif