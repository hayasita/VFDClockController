/*
  ESP32-S3 I2S Sound Output
    sound.h
*/
#ifndef sound_h
#define sound_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <M5Unified.h>

#define USE_LittleFS

#include <FS.h>
#ifdef USE_LittleFS
  #define SPIFFS LittleFS
  #include <LittleFS.h> 
#else
  #include <SPIFFS.h>
#endif 

class i2sS3Driver{
  public:
    void init(void);        // 初期化
    void soundPipo(void);   // ピポ音出力
    void soundWestminsterChimes(void);        // ウェストミンスターの鐘出力
    bool playFileWave(const char *filename);  // wavefile 再生

    void test(void);
  private:

};
static constexpr const size_t buf_num = 2;
static constexpr const size_t buf_size = 256;
static uint8_t wav_data[buf_num][buf_size];

GLOBAL i2sS3Driver i2sS3Sound;

// Mailbox用のキュー
#define QUEUE_SOUNDLENGTH 4
GLOBAL QueueHandle_t xQueueSoundPlay;

GLOBAL TaskHandle_t sountaskHandle;             // サウンド処理タスクハンドル
GLOBAL void taskSoundCtrl(void *pvParameters);  // SoundCtrlタスク CPU1

#undef GLOBAL
#endif
