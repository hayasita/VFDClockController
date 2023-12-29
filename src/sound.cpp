/*
  ESP32-S3 I2S Sound Output
    sound.cpp
*/

#include "sound.h"
#include "esp_log.h"

// 初期化
void i2sS3Driver::init(void)
{
  // キュー作成
  xQueueSoundPlay = xQueueCreate(QUEUE_SOUNDLENGTH, sizeof(uint8_t));

// Sound setup
  auto spkCfg = M5.Speaker.config();
  spkCfg.pin_bck = GPIO_NUM_2;
  spkCfg.pin_ws = GPIO_NUM_1;
  spkCfg.pin_data_out = GPIO_NUM_5;
  spkCfg.magnification = 48;             // 音量設定
  spkCfg.i2s_port = I2S_NUM_1;
  M5.Speaker.config(spkCfg);
  M5.Speaker.begin();

  return;
}

// ピポ音出力
void i2sS3Driver::soundPipo(void)
{
  M5.Speaker.tone(2000, 100, 0, false);
  M5.Speaker.tone(1000, 100, 0, false);

  return;
}

// ウェストミンスターの鐘出力
void i2sS3Driver::soundWestminsterChimes(void)
{
    M5.Speaker.tone(698.451, 400, 0, true);   // ファの音
    M5.Speaker.tone(880.000, 400, 0, false);  // ラの音
    M5.Speaker.tone(783.991, 400, 0, false);  // ソの音
    M5.Speaker.tone(523.251, 400, 0, false);  // ドの音
    delay(1500);
    M5.Speaker.tone(698.451, 400, 0, false);  // ファの音
    M5.Speaker.tone(783.991, 400, 0, false);  // ソの音
    M5.Speaker.tone(880.000, 400, 0, false);  // ラの音
    M5.Speaker.tone(698.451, 400, 0, false);  // ファの音
    delay(1500);
    M5.Speaker.tone(880.000, 400, 0, false);  // ラの音
    M5.Speaker.tone(698.451, 400, 0, false);  // ファの音
    M5.Speaker.tone(783.991, 400, 0, false);  // ソの音
    M5.Speaker.tone(523.251, 400, 0, false);  // ドの音
    delay(1500);
    M5.Speaker.tone(523.251, 400, 0, false);  // ドの音
    M5.Speaker.tone(783.991, 400, 0, false);  // ソの音
    M5.Speaker.tone(880.000, 400, 0, false);  // ラの音
    M5.Speaker.tone(698.451, 400, 0, false);  // ファの音

  return;
}
void i2sS3Driver::test(void)
{

  return;
}

// wavefile 再生
bool i2sS3Driver::playFileWave(const char *filename)
{
  struct __attribute__((packed)) wav_header_t
  {
    char RIFF[4];
    uint32_t chunk_size;
    char WAVEfmt[8];
    uint32_t fmt_chunk_size;
    uint16_t audiofmt;
    uint16_t channel;
    uint32_t sample_rate;
    uint32_t byte_per_sec;
    uint16_t block_size;
    uint16_t bit_per_sample;
  };
  struct __attribute__((packed)) sub_chunk_t
  {
    char identifier[4];
    uint32_t chunk_size;
    uint8_t data[1];
  };

  auto file = SPIFFS.open(filename);

  if (!file) {
    Serial.println( "-- SPIFFS Open Error!!" );
    return false; 
  }

  wav_header_t wav_header;
  file.read((uint8_t*)&wav_header, sizeof(wav_header_t));
/*
  ESP_LOGD("wav", "RIFF           : %.4s" , wav_header.RIFF          );
  ESP_LOGD("wav", "chunk_size     : %d"   , wav_header.chunk_size    );
  ESP_LOGD("wav", "WAVEfmt        : %.8s" , wav_header.WAVEfmt       );
  ESP_LOGD("wav", "fmt_chunk_size : %d"   , wav_header.fmt_chunk_size);
  ESP_LOGD("wav", "audiofmt       : %d"   , wav_header.audiofmt      );
  ESP_LOGD("wav", "channel        : %d"   , wav_header.channel       );
  ESP_LOGD("wav", "sample_rate    : %d"   , wav_header.sample_rate   );
  ESP_LOGD("wav", "byte_per_sec   : %d"   , wav_header.byte_per_sec  );
  ESP_LOGD("wav", "block_size     : %d"   , wav_header.block_size    );
  ESP_LOGD("wav", "bit_per_sample : %d"   , wav_header.bit_per_sample);
*/
  if ( memcmp(wav_header.RIFF,    "RIFF",     4)
    || memcmp(wav_header.WAVEfmt, "WAVEfmt ", 8)
    || wav_header.audiofmt != 1
    || wav_header.bit_per_sample < 8
    || wav_header.bit_per_sample > 16
    || wav_header.channel == 0
    || wav_header.channel > 2
    )
  {
    file.close();
    return false;
  }

  file.seek(offsetof(wav_header_t, audiofmt) + wav_header.fmt_chunk_size);
  sub_chunk_t sub_chunk;

  file.read((uint8_t*)&sub_chunk, 8);
/*
  ESP_LOGD("wav", "sub id         : %.4s" , sub_chunk.identifier);
  ESP_LOGD("wav", "sub chunk_size : %d"   , sub_chunk.chunk_size);
*/
  while(memcmp(sub_chunk.identifier, "data", 4))
  {
    if (!file.seek(sub_chunk.chunk_size, SeekMode::SeekCur)) { break; }
    file.read((uint8_t*)&sub_chunk, 8);
/*
    ESP_LOGD("wav", "sub id         : %.4s" , sub_chunk.identifier);
    ESP_LOGD("wav", "sub chunk_size : %d"   , sub_chunk.chunk_size);
*/
  }

  if (memcmp(sub_chunk.identifier, "data", 4))
  {
    file.close();
    return false;
  }

  int32_t data_len = sub_chunk.chunk_size;
  bool flg_16bit = (wav_header.bit_per_sample >> 4);

  size_t idx = 0;
  while (data_len > 0) {
    size_t len = data_len < buf_size ? data_len : buf_size;
    len = file.read(wav_data[idx], len);
    data_len -= len;

    if (flg_16bit) {
      M5.Speaker.playRaw((const int16_t*)wav_data[idx], len >> 1, wav_header.sample_rate, wav_header.channel > 1, 1, 0);
    } else {
      M5.Speaker.playRaw((const uint8_t*)wav_data[idx], len, wav_header.sample_rate, wav_header.channel > 1, 1, 0);
    }
    idx = idx < (buf_num - 1) ? idx + 1 : 0;
  }
  file.close();

  return true;
}

#ifdef DELETE
// SoundCtrlタスク CPU1 
void taskSoundCtrl(void *pvParameters) {
  uint32_t ulNotifiedValue;
  while (1) {
    // 通知が来るまで待機する。値のクリアはしない
    xTaskNotifyWait( 0,
                     0,
                     &ulNotifiedValue,
                     portMAX_DELAY );
    Serial.println( "-- taskSoundCtrl" );
//    M5.Speaker.setVolume(80);
//    if(playSdWav("/wav/003.wav")){
    if(i2sS3Sound.playFileWave("/wav/west.wav")){
      Serial.println( "-- playSdWav OK." );
    }
    else{
      Serial.println( "-- playSdWav NG!" );
    }

    i2sS3Sound.soundWestminsterChimes();

  }
}
#endif

// SoundCtrlタスク CPU1 
void taskSoundCtrl(void *pvParameters) {
  uint8_t ret = 0;
  uint8_t data;

  while (1) {
    ret = xQueueReceive(xQueueSoundPlay, &data, 0);
    if(ret){
      Serial.printf("%d = xQueueReceive() : ret = %d\n", data, ret);
      if(data == 1){
        if(i2sS3Sound.playFileWave("/wav/west.wav")){
          Serial.println( "-- playSdWav OK." );
        }
        else{
          Serial.println( "-- playSdWav NG!" );
        }
      }
      else if(data == 2){
        i2sS3Sound.soundWestminsterChimes();
      }
    }
    delay(1);
  }

}
