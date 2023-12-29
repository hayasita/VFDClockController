/**
 * @file vfd_conf.h
 * @author hayasita04@gmail.com
 * @brief 設定値作成・Logデータ管理
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef vfd_conf_h
#define vfd_conf_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <M5Unified.h>
#include <vector>
#include <time.h>

#define VERSION "0.01.00S"      // SWバージョン 0.01.00S

#define ON 1
#define OFF 0

#define DISP_KETAMAX  9   // VFD表示桁数

#define BR_DEF        9   // 輝度初期値
#define BR_MAX        15  // 最大輝度
#define BR_MIN        1   // 最小輝度
#define ADJ_BR1       0

#define FADETIME_DEF  2   // クロスフェード時間初期値

#define EEROM_HEADER  0x5A
#define EEROM_CONFIG  0x0B

struct HEADER_DATA {
  uint8_t eerom_header;   // EEPROMヘッダ
  char version[10];       // SWバージョン
};
struct CONFIG_DATA {      // 動作設定値
  char staSsid[33];               // WiFi Station mode SSID
  char staSsidPassword[64];       // WiFi Station mode SSID Password
  uint8_t localesId;              // 地域設定
  uint8_t ntp_setw;               // 0x01 ntp使用選択　使用
  uint8_t timeZoneData;           // タイムゾーン設定
  uint8_t timeZoneId;             // タイムゾーンID
  uint8_t timeZoneAreaId;         // タイムゾーンエリアID
  uint8_t autoUpdateHourw;        // 自動更新時刻：時
  uint8_t autoUpdateMinw;         // 自動更新時刻：分

  uint8_t format_hw;              // 時刻表示フォーマット 12/24H
  uint8_t dispFormat;             // 表示フォーマット設定
  uint8_t timeDisplayFormat;      // 時刻表示フォーマット設定
  uint8_t dateDisplayFormat;      // 日付表示フォーマット設定
  uint8_t displayEffect;          // 表示効果
  uint8_t fadetimew;              // クロスフェード時間(1~9)
  uint8_t br_dig[DISP_KETAMAX];   // 0x02 - 0x0A 表示各桁輝度
  uint8_t glowInTheBright;        // 全体輝度設定値：明
  uint8_t glowInTheDark;          // 全体輝度設定値：暗

  uint8_t staAutoConnectSel;      // STA自動接続有効無効

//  uint8_t eeromEventStartNum;     // EEPROMイベント情報開始番号
  uint8_t eeromEventDataNum;      // EEPROMイベント情報個数
  uint8_t eeromEventWriteNum;     // EEPROMイベント書き込み番号
};

class configVFD{
  public:
    configVFD();
    void init(void);
//    void eeromWrite(void);                // EEPROM/FPIFFS書き込み 未使用
//    void eeromConfWrite(void);            // EEPROM書き込み 未使用
    void eeromDataout(void);              // ConfigData表示
    void eeromTmpMake(void);              // Config TmpData作成

    void i2cEepromIni(void);              // ConfigData i2cEEPROM初期値書込
    void i2cEepromRead(void);             // ConfigData i2cEEPROM値読み出し
    void i2cEepromWrite(void);            // i2cEEPROM書込処理 

    void resetTmp(void);                  // ConfigDataTmpクリア
    void writeTmp(void);                  // ConfigDataTmp設定

    void setConfigDefault(void);          // ConfigData初期値設定　todo.i2cEepromIni()と統合

    void setConfigHeader(uint8_t);        // EEPROM header設定
    uint8_t getConfigHeader(void);        // EEPROM header取得
    void setVersion(char *);              // version設定
    char *getVersion(void);               // version取得

    // SSID,Password WebIFからの受け渡し用
    void clearStaData(void);              // WiFi Station mode SSID,Password削除
    char *getSsid(void);                  // WiFi Station mode SSID取得
    void setSsid(char *);                 // WiFi Station mode SSID設定
    char *getSsidPass(void);              // WiFi Station mode SSID Password取得
    void setSsidPass(char *);             // WiFi Station mode SSID Password設定

    // ナビゲーションバー
    uint8_t getLocalesId(void);           // 地域設定取得
    void setLocalesId(uint8_t);           // 地域設定設定
  
    // 時刻設定情報
    uint8_t getNtpset(void);              // ntp使用選択情報取得
    void setNtpset(uint8_t datw);         // ntp使用選択情報設定

    const char *getTimeZoneData(void);    // タイムゾーン設定取得(GMT名称)
    uint8_t getTimeZoneDataIndex(void);   // タイムゾーンGMT名称のindex
    void setTimeZoneData(uint8_t);        // タイムゾーン設定設定(GMT名称のindex)
    uint8_t getTimeZoneId(void);          // タイムゾーンID設定取得
    void setTimeZoneId(uint8_t);          // タイムゾーンID設定設定
    uint8_t getTimeZoneAreaId(void);      // タイムゾーンエリアID設定取得
    void setTimeZoneAreaId(uint8_t);      // タイムゾーンエリアID設定設定

    void getAutoUpdatetime(uint8_t *, uint8_t *); // 自動更新時刻取得
    void setAutoUpdatetime(uint8_t, uint8_t);     // 自動更新時刻設定
    
//    void getLastUpdate(uint16_t *,uint8_t *,uint8_t *,uint8_t *,uint8_t *,uint8_t *);// 最終更新時刻取得
//    void setLastUpdate(tm timeInfo);      // 最終更新時刻設定

    // 表示設定
    uint8_t GetFormatHw(void);            // 時刻表示フォーマット情報取得
    uint8_t GetFormatHwTmp(void);         // 時刻表示フォーマットtmp情報取得
    void SetFormatHw(uint8_t fw);         // 
    void SetFormatHwTmp(uint8_t fw);
    void FormatHchg(void);                // 時刻表示フォーマットtmp切替

    uint8_t GetdispFormatw(void);         // 表示フォーマット設定情報取得
    void SetdispFormatw(uint8_t mode);    // 表示フォーマット設定情報記録

    uint8_t getTimeDisplayFormat(void);   // 時刻表示フォーマット設定取得
    void setTimeDisplayFormat(uint8_t);   // 時刻表示フォーマット設定記録
    uint8_t getDateDisplayFormat(void);   // 日付表示フォーマット設定取得
    void setDateDisplayFormat(uint8_t);   // 日付表示フォーマット設定記録

    uint8_t getDisplayEffect(void);       // 表示効果取得
    void setDisplayEffect(uint8_t dat);   // 表示効果設定

    uint8_t GetFadetimew(void);           // クロスフェード時間情報取得
    uint8_t GetFadetimewTmp(void);        // クロスフェード時間tmp情報取得
    void SetFadetimew(uint8_t tw);        // クロスフェード時間情報記録
    void SetFadetimewTmp(uint8_t tw);     // クロスフェード時間tmp情報記録

    void FadetimeAdd(void);               // クロスフェード時間＋
    void FadetimeDec(void);               // クロスフェード時間－

    uint8_t *GetBr_digp(void);            // 輝度情報ポインタ取得
    uint8_t *GetBr_digTmpp(void);         // 輝度情報tmpポインタ取得
    uint8_t GetBr_dig(uint8_t adj_point);       // 輝度情報個別取得
    uint8_t GetBr_digtmp(uint8_t adj_point);    // 輝度情報個別取得

    uint8_t GetGlowInTheBright(void);           // 全体輝度設定値：明情報取得
    uint8_t GetGlowInTheDark(void);             // 全体輝度設定値：暗情報取得
    uint8_t GetGlowInTheBrightTmp(void);        // 全体輝度設定値：明情報取得
    uint8_t GetGlowInTheDarkTmp(void);          // 全体輝度設定値：暗情報取得
    void SetGlowInTheBrightTmp(uint8_t data);   // 全体輝度設定値：明情報設定
    void SetGlowInTheDarkTmp(uint8_t data);     // 全体輝度設定値：暗情報設定
    void submitGlowInTheBright(void);           // 全体輝度設定値：明情報記録
    void submitGlowInTheDark(void);             // 全体輝度設定値：明情報記録

    void SetBr_dig(uint8_t adj_point,uint8_t brw);    // 輝度情報個別設定
    void SetBr_digtmp(uint8_t adj_point,uint8_t brw); // 輝度情報個別設定
    void Br_digAdd(uint8_t adj_point);                // 輝度情報＋
    void Br_digDec(uint8_t adj_point);                // 輝度情報－

    // WiFi設定
    uint8_t getStaAutoConnectSel(void);           // STA自動接続有効無効取得
    void setStaAutoConnectSel(uint8_t data);      // STA自動接続有効無効記録

    // イベント情報
//    uint8_t getEeromEventStartNum(void);          // EEPROMイベント情報開始番号取得
//    void setEeromEventStartNum(uint8_t data);     // EEPROMイベント情報開始番号記録

    uint8_t getEeromEventDataNum(void);           // EEPROMイベント情報個数取得
    void setEeromEventDataNum(uint8_t data);      // EEPROMイベント情報個数記録
    uint8_t getEeromEventWriteNum(void);          // EEPROMイベント書き込み番号取得
    void setEeromEventWriteNum(uint8_t data);     // EEPROMイベント書き込み番号記録

    // 画面表示設定
    void setTabName(const char *);      // Web表示タブ名称設定
    String getTabName(void);            // Web表示タブ名称取得
    uint8_t getTabNumber(void);         // Web表示タブ番号取得

    uint8_t dispFormatUpdatef;      // 表示フォーマット設定更新フラグ

    // debug
    void debugI2cWritePr(void);

  private:
    struct HEADER_DATA HeaderData;      // EEROMヘッダ
    struct CONFIG_DATA configData;      // 設定データ
    struct CONFIG_DATA configTmp;       // 設定データtmp

    bool i2cEeromWriteReq;              // EEPROM書き込みリクエスト
    bool i2cEeromWriteReqAdr[0x100];    // EEPROM書き込みアドレス
    void i2cEepromWriteReq(uint16_t);   // EEPPROM書き込み要求設定

    String tabName;                     // 選択中のタブ名称
    uint8_t tabNumber;                  // 選択中のタブ番号

    portMUX_TYPE mutexEeromWrReq = portMUX_INITIALIZER_UNLOCKED;  // EEPROM設定情報記録要求用
    portMUX_TYPE mutex_conf = portMUX_INITIALIZER_UNLOCKED; // Mutex
};

//GLOBAL uint8_t (configVFD::*pGetFadetimew)() = &configVFD::GetFadetimew;
GLOBAL configVFD confDat;

//
// -- イベントLOG --
//
#define EVENT_LOGMAX        20      /*!< @brief イベントログ最大記憶数（旧方式 */

#define EVENT_LOGSIZE       16      /*!< @brief イベントログ1つのバイト数 */
//#define EVENT_LOGBUFMAX     8       // イベントログのバッファ書き出し個数
#define EVENT_LOGBUFMAX     7       // イベントログのバッファ書き出し個数 Debug用
#define EVENT_LOGREADMAX    8 //2       // 1度に読み出す最大イベントLOG数

#define EVENT_TEST          0x00    // テスト用

// Device設定
#define EVENT_BOOT_TASKDEVICECTRL         0x10    // 始動
#define EVENT_EVENT_DELAYED_WRITE         0x11    // イベント遅延書き込み

#define EVENT_RTCSET                      0x20    // RTC時刻設定
#define EVENT_WIFI_AUTOCON_SNTP_REQ       0x21    // SNTP処理要求
#define EVENT_SNTP_AUTOCON_TIME_REQ       0x22    // SNTP取得　設定時刻接続動作要求
#define EVENT_SNTP_AUTOCON_TIME_CONP      0x23    // SNTP取得　接続動作完了
#define EVENT_SNTP_STARTCON_REQ           0x24    // SNTP取得　起動時接続動作要求

// WiFi動作
#define EVENT_WIFI_AUTOCON_START          0x80  // WiFi自動接続
#define EVENT_WIFI_AUTOCON_COMP           0x81  // WiFi自動接続完了
#define EVENT_WIFI_AUTOCON_SNTP_COMP      0x82  // SNTP処理完了
#define EVENT_WIFI_AUTOCON_SNTP_TIMEOUT   0x83  // SNTP処理タイムアウト
	
#define EVENT_WIFI_MANCON_APSTART         0x84  // 手動接続・WiFi APモード起動
#define EVENT_WIFI_MANCON_APSTARTFAIL     0x85  // 手動接続・WiFi APモード起動失敗
#define EVENT_WIFI_MANCON_APCOMP          0x86  // 手動接続・WiFi APモード起動完了
#define EVENT_WIFI_MANCON_STASTART        0x87  // 手動接続・WiFi STA起動
#define EVENT_WiFi_MANCON_STACOMP         0x88  // 手動接続・WiFi STA接続完了
	
#define EVENT_WiFi_STA_RECON_DISCON       0x89  // STA再接続・切断
#define EVENT_WiFi_STA_RECON_CON_START    0x8A  // STA再接続・接続
#define EVENT_WiFi_STA_RECON_SSID_DELETE  0x8B  // SSID削除
#define EVENT_WiFi_STA_RECON_COMP         0x8C  // STA再接続・接続完了

#define EVENT_WiFi_DISCON                 0x8D  // WiFI切断
#define EVENT_WiFi_STAERR_TIMEOUT         0x8E  // STA接続エラー・タイムアウト
#define EVENT_WiFi_MAN_DISCON             0x8F  // WiFI手動中断
	
#define EVENT_WiFi_SSIDSCAN_START         0xA0  // WiFi SSID 検索
#define EVENT_WiFi_SSIDSCAN_COMP          0xA1  // WiFi SSID 検索完了

struct EVENT_LOG{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint8_t event;
  uint16_t data[4];
};

struct eventQueData{
  uint8_t dataSize;                       // データ数
  EVENT_LOG eventData[EVENT_LOGREADMAX];  // 送信データ
};

/**
 * @class eventLog 
 * @brief イベントログの作成・読み出し
 * 
 */
class eventLog{
  public:
    eventLog();   /*!< @brief Construct a new event Log object */

    // イベント記録新方式

    void setEventlogLoop(uint8_t event,uint16_t* data = nullptr);       /*!< @brief loopタスクイベントLogバッファ記録 */
    void setEventlogDeviceCtrl(uint8_t event,uint16_t* data = nullptr); /*!< @brief DeviceCtrlタスクイベントLogバッファ記録 */

    void eventBuf2Eeprom(void);                 /*!< @brief イベントLogバッファからEEPROMへ書き込み */
    uint8_t getEepromLogBufferSize(void);       /*!< @brief EEPROM書き込みバッファ数取得 */
    uint8_t getEepromLogBufferMaxSize(void);    /*!< @brief EEPROM書き込みバッファ最大数取得 */
    struct tm getLastWriteTimeEp(void);         /*!< @brief 前回書き込み時刻取得 */

    uint8_t eventSend(struct eventQueData *);   /*!< @brief イベントLogをEEPROMとバッファから読み出す */
    uint8_t eventLogSendSqf;                    /*!< @brief イベントLOG送信要求シーケンスフラグ */

    void putLog(EVENT_LOG);                     /*!< @brief EVENT_LOGを表示 */

  private:
    void setEventlog(uint8_t event,uint16_t* data = nullptr);       /*!< @brief イベントログ記録 */
    SemaphoreHandle_t logSemapho;                                   /*!< @brief イベントログ記録セマフォハンドル */

    // イベント記録新方式
    portMUX_TYPE mutexEeprom = portMUX_INITIALIZER_UNLOCKED;        /*!< @brief EEPROMイベント記録用 mutex */
    std::vector<EVENT_LOG> eepromLogBuffer;                         /*!< @brief EEPROM書き込み用イベントバッファ */
//    uint8_t eepromLogBufp;                    /*!< @brief EEPROM書き込み用イベントバッファポインタ */
    inline bool setEepromLogBuf(EVENT_LOG);                         /*!< @brief EEPROM書き込み用イベントバッファLog記録 */
    bool getEepromLogBuf(EVENT_LOG *,uint8_t);                      /*!< @brief バッファの中で最も古いイベントを返す。 */
    uint8_t eepromLogBufferMaxSize;                                 /*!< @brief バッファに記録された最大数 */
    void writeEepromEvent(std::vector<EVENT_LOG>,uint8_t,uint8_t);  /*!< @brief イベントをEEPROMに書き込む */
    unsigned long lastWriteTime;                                    /*!< @brief 前回書き込み時間 */

    
    struct tm lastWriteTimeEp;      /*!< @brief 前回書き込み時刻 */

    // イベントLOGバッファからEEPROMに書き込み
    uint8_t eventBuf2EepromSqf;     /*!< @brief イベントLogバッファEEPROMへ書き込みシーケンスフラグ */
#define EVENT_BUF2EEROM_WAIT  0     /*!< @brief 待機 */
#define EVENT_BUF2EEROM_WRITE 1     /*!< @brief 書き込み開始 */
#define EVENT_BUF2EEROM_WLOOP 2     /*!< @brief 書き込みループ実行中 */
    uint8_t eeromWriteDataNum;      /*!< @brief イベントデータ書き込み数 */
    uint8_t eeromWriteNum;          /*!< @brief イベント書き込みバッファ番号 */

    // イベントLOGバッファ読み出し
//    uint8_t eventLogSendSqf;          /*!< @brief イベントLOG送信要求シーケンスフラグ */
#define EVENT_LOGSEND_WAIT        0   /*!< @brief 待機 */
#define EVENT_LOGSEND_E2ROM_PRE   1   /*!< @brief EEPROMログ送信準備 */
#define EVENT_LOGSEND_E2ROM_EXEC  2   /*!< @brief EEPROMログ送信実行 */
#define EVENT_LOGSEND_BUF_EXEC    3   /*!< @brief イベントバッファログ送信実行 */
#define EVENT_LOGSEND_STANDBY     4   /*!< @brief イベントLOG送信待機 */

    uint8_t logBufferReadNum;         /*!< @brief イベントLOGバッファ読み出し位置 */
    uint8_t eeromReadDataNum;         /*!< @brief イベントデータ読み出し数 */
    uint8_t StartBufReadNum;          /*!< @brief 送信開始バッファ番号 */

};
GLOBAL eventLog vfdevent;

GLOBAL bool eventSntpSync;              // SNTP同期完了イベント要求

GLOBAL bool eventAutoConSntpReq;        // 0x21:SNTP処理要求イベント要求

GLOBAL bool confWiFiStartf;             // WiFi手動接続要求


#undef GLOBAL
#endif