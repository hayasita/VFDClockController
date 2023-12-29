#ifndef eerom_h
#define eerom_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <M5Unified.h>
#include <wire.h>

#define ADR_eerom_header      0x00
#define ADR_version           0x01

#define ADR_format_hw         0x10
#define ADR_dispFormat        0x11
#define ADR_timeDisplayFormat 0x12
#define ADR_dateDisplayFormat 0x13
#define ADR_displayEffect     0x14
#define ADR_fadetime          0x15
#define ADR_glowInTheBright   0x16
#define ADR_glowInTheDark     0x17
#define ADR_br_dig0           0x18
#define ADR_br_dig1           0x19
#define ADR_br_dig2           0x1A
#define ADR_br_dig3           0x1B
#define ADR_br_dig4           0x1C
#define ADR_br_dig5           0x1D
#define ADR_br_dig6           0x1E
#define ADR_br_dig7           0x1F
#define ADR_br_dig8           0x20

#define ADR_ntp_setw          0x30
#define ADR_timeZoneAreaId    0x31
#define ADR_timeZoneId        0x32
#define ADR_timeZone          0x33
#define ADR_autoUpdateHourw   0x34
#define ADR_autoUpdateMinw    0x35
#define ADR_localesId         0x3C

#define ADR_staAutoConnect    0x3D

//#define ADR_eeromEventStartNum  0x40
#define ADR_eeromEventDataNum   0x40
#define ADR_eeromEventWriteNum  0x41


#define ADR_eventLogStart     0x0100  // イベントログEEPROM記録先頭アドレス
#define EVENTLOG_NUM          0x80    // イベントログEEPROM記録件数

#define E2R_I2CADR            0x50    // E2PROM i2cアドレス
//#define E2R_PAGESIZE          16      // E2PROMページサイズ
#define E2R_PAGESIZE          32      // E2PROMページサイズ
#define E2R_SIZE              0x2000  // E2PROMサイズ

struct I2CEEROM_DATA{
  uint16_t  addr;         // 開始アドレス
  uint8_t   defaultNum;   // 初期値
  uint8_t   minNum;       // 最小値
  uint8_t   maxNum;       // 最大値

  void  (*writefunc)(uint8_t);    // EEROM書込関数
  uint8_t (*readConf)(void);      // Config設定値読出関数
  void (*defSetConf)(uint8_t);    // 初期値取得関数

};

class EEROM_CTRL{
  public:
    EEROM_CTRL();
    uint8_t conectChk(void);
    void write(uint8_t, uint16_t, uint8_t );
    void sequentialWrite(uint8_t , uint16_t  ,uint8_t *,uint8_t );
    void iniWrite(uint16_t);
    uint8_t read(uint8_t, uint16_t);
    void sequentialRead(uint8_t, uint16_t,uint8_t *,uint8_t);
    void printADR(uint16_t);

    void allclear(void);                      // E2ROMデータクリア 
    void dump(uint16_t,uint16_t);             // E2ROMデータ表示

  private:
    uint8_t eeromConent;

};
GLOBAL EEROM_CTRL eerom;

#undef GLOBAL
#endif