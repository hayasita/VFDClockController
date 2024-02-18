#ifndef vfd_driver_h
#define vfd_driver_h

// GLOBALを使用するヘッダはここでincludeする。
#include "driver_sub.h"
#include "mode_ctrl.h"

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <vector>

#include <M5Unified.h>
#include <M5UnitOLED.h>
#include "InputTerminal.h"
#include "M5_ENV.h"
// OLED設定
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

GLOBAL void oledDispInit(void);
GLOBAL void oledDispTime(tm rtcTimeInfo,tm localTimeinfo);

#ifdef ARDUINO_M5Stack_Core_ESP32
//  #include <M5Stack.h>
  #define BORDNAME "M5Stack"
#elif ARDUINO_M5Stick_C
//  #include <M5StickCPlus.h>
  #define BORDNAME "M5StickCPlus"

#elif ARDUINO_M5Stack_ATOM
//  #include <M5Atom.h>
//  #define BORDNAME "M5Atom"

  #define USE_VFD
  #define SPISCLK_PIN 22
  #define SPISS_PIN   19
  #define SPIMOSI_PIN 25

  #define USE_DCDC
  #define DCDCFDB_PIN 33
  #define DCDCPWM_PIN 21

  #define DCDC_TargetV  1800       // DCDC目標電圧 30.5v

  #define ILLUMI_PIN  34          //ATOM Light
//  #define ILLUMI_PIN  36           //Stamp Pico

// -- M5ATOM GROVE接続端子使用ユニット選択 --
  #define USE_I2C               // I2Cユニット
  #define SCL_PIN     32
  #define SDA_PIN     26
  #define I2C_ADDRESS_DS1307  0x68   //I2C address for DS1307
  #define I2C_ADDRESS_24C32   0x50   //I2C address for 24C32

//  #define USE_IR                  // 赤外線リモコンユニット
  #define IRRCV_PIN   32

// LED設定
  #define NUM_LEDS 1                // RGB LEDの数を指定(M5Atom Matrixなら25)
  #define LED_DATA_PIN 27           // RGB LEDのDATA PINを指定
#else
//  #include <M5Atom.h>
//  #define BORDNAME "M5AtomS3"
/*
  #define USE_VFD
  #define SPISCLK_PIN 5   //22
  #define SPISS_PIN   6   //19
  #define SPIMOSI_PIN 38  //25

  #define USE_DCDC
  #define DCDCFDB_PIN 8   //33
  #define DCDCPWM_PIN 39  //21

  #define DCDC_TargetV  1800       // DCDC目標電圧 30.5v

  #define ILLUMI_PIN  7   //34          //ATOM Light
//  #define ILLUMI_PIN  36           //Stamp Pico

// -- M5ATOM GROVE接続端子使用ユニット選択 --
//  #define USE_I2C               // I2Cユニット
  #define SCL_PIN     1   //32
  #define SDA_PIN     2   //26
  #define I2C_ADDRESS_DS1307  0x68   //I2C address for DS1307
  #define I2C_ADDRESS_24C32   0x50   //I2C address for 24C32

  #define USE_IR                  // 赤外線リモコンユニット
  #define IRRCV_PIN   1   //32

// LED設定
  #define NUM_LEDS 1                // RGB LEDの数を指定(M5Atom Matrixなら25)
  #define LED_DATA_PIN 35   //27           // RGB LEDのDATA PINを指定
*/

  #define BORDNAME "M5StampS3"

  #define USE_VFD               // VFD表示使用
  #define SPISCLK_PIN   11
  #define SPISS_PIN     6
  #define SPIMOSI_PIN   4
  #define VFD_BLANKING  39

  #define USE_DCDC              // DCDCコンバータ使用
  #define DCDCFDB_PIN   8       // DCDCコンバータフィードバックアナログ入力
  #define DCDCPWM_PIN   3       // DCDCコンバータPWM出力

  #define DCDC_TargetV  1800    // DCDC目標電圧 30.5v

  #define ILLUMI_PIN    7       // 照度センサ入力

  #define USE_I2C               // I2Cユニット使用
  #define SCL_PIN       15      // SCL
  #define SDA_PIN       13      // SDA
  #define I2C_ADDRESS_M5OLED    0x3C    //I2C address for M5 OLED Display SH1107
  #define I2C_ADDRESS_SSD1306   0x3D    //I2C address for SSD1306 OLED Display
  #define I2C_ADDRESS_DS1307    0x68    //I2C address for DS1307
  #define I2C_ADDRESS_24C32     0x50    //I2C address for 24C32
  #define I2C_ADDRESS_SHT30     0x44    //I2C address for SHT30
  #define I2C_ADDRESS_QMP6988   0x70    //I2C address for QMP6988
  #define I2C_ADDRESS_BME680    0x77    //I2C address for BME680

  #define USE_IR                // 赤外線リモコンユニット使用
  #define IRRCV_PIN     46

// LED設定
//  #define NUM_LEDS      1       // 内蔵RGBLEDの数
//  #define LED_DATA_PIN  21      // 内蔵RGBLEDのDATA PIN
  #define NUM_LEDS      2       // 内蔵RGBLEDの数
  #define LED_DATA_PIN  14      // 内蔵RGBLEDのDATA PIN

#endif

#include <FastLED.h>
#include <wire.h>
//#include <RTC8564_esp32.h>
//#include <DS1307RTC_esp32.h>
//#include "time.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define BUTTON_0 41
#define BUTTON_1 0
#define BUTTON_2 9
#define BUTTON_3 10
uint8_t itmMan(void);
// i2c Scan
GLOBAL void i2cScan(void);

// --Device検出情報
class I2CDeviceDetection{
  public:
    bool datRtc;          // RTC
    bool datE2ROM;        // EEPROM
    bool datSHT30;        // SHT30
    bool datQMP6988;      // QMP6988
    bool datSSD1306;      // SSD1306 OLED Display
    bool datM5OLED;       // M5 OLED Unit
    bool datBME680;       // BME680

};

// -- Device有無確認
class DeviceChk{
  public:
    void init(void);
    uint8_t rtc(void);
    bool e2rom(void);
    bool sht30(void);
    bool qmp6988(void);
    bool ssd1306(void);
    bool m5oled(void);
    bool bme680(void);

    void i2cScan();
    std::vector<uint8_t> i2cDevice;
    I2CDeviceDetection detection;   // i2cDevice 検出情報

  private:
    uint8_t wireChk(uint8_t address);
    String getBoardName(void);

};
GLOBAL  DeviceChk deviceChk;

// i2cdevice ステータス表示用
class DevicePresence{
  public:
    I2CDeviceDetection i2c;   // i2cDevice 検出情報
    dispMode displayMode; // Display Mode Ctrl

};

// -- デバッグ用表示情報
class DisplayData{
  public:
    struct tm timeInfo;
    struct tm rtcTimeInfo;
    DeviceData deviceDat;
};

// -- LED制御 --

#define LEDMODE_WIFI_NO_CONNECT               0   // WiFi接続なし
#define LEDMODE_SNTP_STARTUP_CONNECT_START    1   // 起動時SNTP接続開始~接続完了
#define LEDMODE_SNTP_STARTUP_CONNECT          2   // 起動時SNTP接続完了・同期中
#define LEDMODE_SNTP_SYNC_COMPLETED           3   // 起動時SNTP同期完了~WiFi切断
#define LEDMODE_WIFI_MANUAL_CONNECT_START     4   // WiFi手動接続開始~接続完了
#define LEDMODE_WIFI_MANUAL_DISDISCONNECT     8   //
#define LEDMODE_WIFI_MANUAL_CONNECT           5   // WiFi手動接続完了~WiFi切断
#define LEDMODE_WIFI_AUTO_CONNECT_START       6   // 時刻合わせWiFi自動接続~接続完了
#define LEDMODE_WIFI_AUTO_CONNECT             7   // 時刻合わせWiFi接続完了~WiFi切断

#define LEDPATTERN_ALLOFF   0   // 消灯
#define LEDPATTERN_ALLON    1   // 常時点灯
#define LEDPATTERN_ONEBEAT  2   // 一拍点滅

#define LED_OFF   0   // LED消灯
#define LED_ON    1   // LED点灯

class LEDControl{
  public:
    LEDControl(void);

    void setLedMode(uint8_t);       // LED動作モード設定
    uint8_t getLedMode(void);       // LED動作モード取得
    void incLedMode(void);          // LED動作モード更新
    void ledMan(void);              // LEDマネージャ
  private:
    uint8_t ledMode;
    uint8_t ledPattern;
    uint8_t ledsqf;
    unsigned long ledLasttime;      // RTC読み込み前回時間(millis)

    CRGB ledColor;
    void LEDSet(CRGB color);        // LED表示色設定
    void LEDSetColor(CRGB color);   // LED表示色設定
    CRGB leds[NUM_LEDS];            // LED操作

#ifdef ARDUINO_M5Stack_ATOM
#elif ARDUINO_M5Stick_C
    uint16_t ledColor;
    void ledOn(void);               // LED点灯
#endif

    void ledOff(void);              // LED消灯
    void setLedPattern(uint8_t);    // 点灯パターン設定

    void makeLedData(void);         // LED動作情報作成

};
GLOBAL LEDControl led;

// -- 光センサ制御 --
//#ifdef ARDUINO_M5Stack_ATOM
class Illuminance{
  public:
    Illuminance(void);
    void ini(uint8_t adpin);
    uint16_t read(void);
    uint16_t getaddat(void);

  private:
    uint8_t illumi_Pin;
    uint16_t addatw;                     // AD入力値
};
GLOBAL Illuminance illumi;
//#endif

// ----- DCDC Control 
/* -- DCDCコンバータ制御 --*/
#define  ON 1
#define  OFF 0

#define DCDC_PWM_PGAIN  60       // Pゲイン
#define DCDC_PWM_IGAIN1  1       // Iゲイン
#define DCDC_PWM_IGAIN2  8       // Iゲイン
#define DCDC_PWM_IGAEN_MAX  200 // Iゲイン最大値
#define DCDC_PWM_MAX  250       // PWM設定値上限

class Dcdccont{
    public:
        Dcdccont(void);         // コンストラクタ
//        void ini(uint8_t fdbpin,uint8_t pwmpin);         //
        void ini(uint8_t fdbpin,uint8_t pwmpin,uint32_t dcdcTargV);
        void ctr(int setpwmw);  //

        int getTarget(uint16_t illumiData);   // 目標値計算取得 

        int getaddat(void);

    private:
        uint8_t dcdc_fdPin;
        unsigned char dcdc_runningf;    // DCDCコンバータ動作許可
        int addatw;                     // AD入力値

        uint8_t glowInTheBright;        // 全体輝度設定値：明情報
        uint8_t glowInTheDark;          // 全体輝度設定値：暗情報
        int dcdcVBright;                // 全体輝度明：目標電圧
        int dcdcVDark;                  // 全体輝度暗：目標電圧
        int dcdcV;                      // DCDCコンバータ目標電圧
};

//GLOBAL Dcdccont dcdcContrl;
/* -- end --*/

// IR
//GLOBAL uint16_t RECV_PIN = IRRCV_PIN;
//GLOBAL IRrecv irrecv(RECV_PIN);
//GLOBAL decode_results results;

GLOBAL void irDump(decode_results *results);

// OLED Disp
/**
 * @brief OLED操作クラス
 * @details SSD1306 OLEDの操作Class
 * 
 */
class OLEDDISP{
  public:
    void init(void);
    void clear(void);
    void printEnvSensorData(DisplayData,dispMode);    // OLED センサデータ表示
    void printEventLog(DisplayData,dispMode);         // OLED イベントログ記録デバッグ情報表示
    void printDeviceData(DevicePresence,dispMode);    // OLED Device情報表示
    void printAnalogData(DisplayData,dispMode);       // OLED アナログデータ表示
  private:
};

// M5 OLED Unit
/**
 * @brief M5OLED操作クラス
 * @details 
 * 
 */
class M5OLED{
  public:
//    M5OLED(void);
    void init(void);
    void clear(void);
    void printEnvSensorData(DisplayData);
  private:
    M5UnitOLED oled;

};

// M5 ENVIII Sensor
class SensorEnviii{
  public:
    void init();
    void read(SensorENVIIIData *sensorDat);
  private:
    SHT3X sht30;
    QMP6988 qmp6988;
};

// BME680
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#define SEALEVELPRESSURE_HPA (1013.25)

class SensorBme680{
  public:
    bool init(SensorBME680Data *bme680Data);            // 初期化
    bool read(SensorBME680Data *bme680Data);            // 同期読み出し
    bool readAsync(SensorBME680Data *bme680Data);       // 非同期読み出し

    bool readAsyncBegin(void);                          // 非同期読み出し準備
    bool readAsyncEnd(SensorBME680Data *bme680Data);    // 非同期読み出し実行

  private:
    Adafruit_BME680 bme;
    unsigned long endTime;    // 非同期読み出し待ち時間格納
};

#undef GLOBAL
#endif
