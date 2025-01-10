#ifndef jsdata_h
#define jsdata_h

#include "vfd_conf.h"

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#ifdef ARDUINO_M5Stack_Core_ESP32
  #include <M5Stack.h>
#elif ARDUINO_M5Stick_C
//  #include <M5StickCPlus.h>
#elif ARDUINO_M5Stack_ATOM
//  #include <M5Atom.h>
#endif
#include <M5Unified.h>
#include <ArduinoJson.h>

String makeSettingjs(void);

//GLOBAL uint8_t settingjsFileWriteReq;
//void settingjsFileWrite(void);

class SettingjsFile{
  public:
    SettingjsFile(void);
    void fileWrite(void);
    void fileRead(configVFD*);

    void writeJsonFile(void);
    void readJsonFile(configVFD*);


    void setWriteReq(uint8_t);
    uint8_t getWriteReq(void);
    void setJsonData(String,configVFD*,uint8_t);
  private:
    uint8_t fileWriteReq;
//    void setJsonData(DynamicJsonDocument doc,configVFD*);
    String makeJsonPiece(String key, String value ,bool connma, bool includeBraces);    // jsonデータ作成:String
    String makeJsonPiece(String key, uint8_t value ,bool connma, bool includeBraces);   // jsonデータ作成:uint8_t
    String makeJsonPiece(String key, std::vector<uint8_t> value ,bool connma, bool includeBraces);  // jsonデータ作成:std::vector<uint8_t>
    String makeJsonPiece(String key, uint16_t value ,bool connma, bool includeBraces);  // jsonデータ作成:uint16_t
    String makeJsonPiece(String key, float value ,bool connma, bool includeBraces);     // jsonデータ作成:float

};
GLOBAL SettingjsFile settingjsFile;

#undef GLOBAL
#endif