/**
 * @file vfd_driver.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "vfd_conf.h"
//#include "vfd_disp.h"
#include "vfd_driver.h"
#include "sound.h"
#include "vfd_web.h"

// 画面表示初期化
//SSD1306Wire display(0x3c, SDA_PIN, SCL_PIN);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h
SSD1306Wire display(I2C_ADDRESS_SSD1306);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

// Adapted from Adafruit_SSD1306
void drawLines() {
  for (int16_t i = 0; i < display.getWidth(); i += 4) {
    display.drawLine(0, 0, i, display.getHeight() - 1);
    display.display();
    delay(10);
  }
  for (int16_t i = 0; i < display.getHeight(); i += 4) {
    display.drawLine(0, 0, display.getWidth() - 1, i);
    display.display();
    delay(10);
  }
  delay(250);

  display.clear();
  for (int16_t i = 0; i < display.getWidth(); i += 4) {
    display.drawLine(0, display.getHeight() - 1, i, 0);
    display.display();
    delay(10);
  }
  for (int16_t i = display.getHeight() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.getHeight() - 1, display.getWidth() - 1, i);
    display.display();
    delay(10);
  }
  delay(250);

  display.clear();
  for (int16_t i = display.getWidth() - 1; i >= 0; i -= 4) {
    display.drawLine(display.getWidth() - 1, display.getHeight() - 1, i, 0);
    display.display();
    delay(10);
  }
  for (int16_t i = display.getHeight() - 1; i >= 0; i -= 4) {
    display.drawLine(display.getWidth() - 1, display.getHeight() - 1, 0, i);
    display.display();
    delay(10);
  }
  delay(250);
  display.clear();
  for (int16_t i = 0; i < display.getHeight(); i += 4) {
    display.drawLine(display.getWidth() - 1, 0, 0, i);
    display.display();
    delay(10);
  }
  for (int16_t i = 0; i < display.getWidth(); i += 4) {
    display.drawLine(display.getWidth() - 1, 0, i, display.getHeight() - 1);
    display.display();
    delay(10);
  }
  delay(250);
}
void printBuffer(void) {
  // Initialize the log buffer
  // allocate memory to store 8 lines of text and 30 chars per line.
  display.setLogBuffer(5, 30);

  // Some test data
  const char* test[] = {
    "Hello",
    "World" ,
    "----",
    "Show off",
    "how",
    "the log buffer",
    "is",
    "working.",
    "Even",
    "scrolling is",
    "working"
  };

  for (uint8_t i = 0; i < 11; i++) {
    display.clear();
    // Print to the screen
    display.println(test[i]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display();
    delay(500);
  }
}

void oledDispInit(void)
{
  if(!deviceChk.ssd1306()){
    Serial.println("OLE Display not found.");
    return;
  }
  // OLED設定
  display.init();

  display.setContrast(255);
/*
  drawLines();
  delay(1000);
  display.clear();
  printBuffer();
  delay(1000);
  display.clear();
*/
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");
  display.display();

  return;
}

extern unsigned long loopTime,loopTimeMax;
extern unsigned long taskDeviceTime,taskDeviceTimeMax;
extern unsigned long taskDeviceTime0,taskDeviceTimeMax0;
extern unsigned long taskDeviceTime1,taskDeviceTimeMax1;
extern unsigned long taskDeviceTime2,taskDeviceTimeMax2;
extern unsigned long taskDeviceTime3,taskDeviceTimeMax3;

extern unsigned long timerExecTime,timerExecTimeMax;
extern unsigned long timerScanTime,timerScanTimeMax;

void oledDispTime(tm rtcTimeInfo,tm localTimeinfo)
{
  if(!deviceChk.ssd1306()){
//    Serial.println("OLE Display not found.");
    return;
  }
  char buffer[100];

  display.clear();
  display.setFont(ArialMT_Plain_10);

  snprintf(buffer, sizeof(buffer),"RTC:%lu/%02d/%02d %02d:%02d:%02d"
    ,rtcTimeInfo.tm_year+1900,rtcTimeInfo.tm_mon+1,rtcTimeInfo.tm_mday
    ,rtcTimeInfo.tm_hour,rtcTimeInfo.tm_min,rtcTimeInfo.tm_sec);
  //Serial.println(buffer);
  display.drawString(0, 0, buffer);

  snprintf(buffer, sizeof(buffer),"Sys:%lu/%02d/%02d %02d:%02d:%02d"
    ,localTimeinfo.tm_year+1900,localTimeinfo.tm_mon+1,localTimeinfo.tm_mday
    ,localTimeinfo.tm_hour,localTimeinfo.tm_min,localTimeinfo.tm_sec);
  //Serial.println(buffer);
  display.drawString(0, 10, buffer);

#ifdef DELETE
/*
  snprintf(buffer, sizeof(buffer),"RTC:%lu/%02d/%02d %02d:%02d:%02d"
    ,rtcTimeInfo.tm_year+1900,rtcTimeInfo.tm_mon+1,rtcTimeInfo.tm_mday
    ,rtcTimeInfo.tm_hour,rtcTimeInfo.tm_min,rtcTimeInfo.tm_sec);
  //Serial.println(buffer);
  display.drawString(0, 0, buffer);

  snprintf(buffer, sizeof(buffer),"LCT:%lu/%02d/%02d %02d:%02d:%02d"
    ,localTimeinfo.tm_year+1900,localTimeinfo.tm_mon+1,localTimeinfo.tm_mday
    ,localTimeinfo.tm_hour,localTimeinfo.tm_min,localTimeinfo.tm_sec);
  //Serial.println(buffer);
  display.drawString(0, 10, buffer);
*/
  // イベントLogバッファサイズ表示
  snprintf(buffer, sizeof(buffer),"LogBuf:%3d MAX:%3d",vfdevent.getEepromLogBufferSize(),vfdevent.getEepromLogBufferMaxSize());
  display.drawString(0, 20, buffer);

  // ヒープサイズ表示
  snprintf(buffer, sizeof(buffer),"Heep:%u",esp_get_free_heap_size());
  display.drawString(0, 30, buffer);

  snprintf(buffer, sizeof(buffer),"Bot:%lu/%02d/%02d %02d:%02d:%02d"
    ,localTimeinfo.tm_year+1900,localTimeinfo.tm_mon+1,localTimeinfo.tm_mday
    ,localTimeinfo.tm_hour,localTimeinfo.tm_min,localTimeinfo.tm_sec);
  //Serial.println(buffer);
  display.drawString(0, 0, buffer);

  // イベントLog前回書き込み時刻
  struct tm timeinfoTmp = vfdevent.getLastWriteTimeEp();
  snprintf(buffer, sizeof(buffer),"BWr:%lu/%02d/%02d %02d:%02d:%02d"
    ,timeinfoTmp.tm_year+1900,timeinfoTmp.tm_mon+1,timeinfoTmp.tm_mday
    ,timeinfoTmp.tm_hour,timeinfoTmp.tm_min,timeinfoTmp.tm_sec);
  display.drawString(0, 10, buffer);

  // 
  snprintf(buffer, sizeof(buffer),"Loop:%4u Max:%u",loopTime,loopTimeMax);
  display.drawString(0, 40, buffer);
  snprintf(buffer, sizeof(buffer),"taskDev:%4u Max:%u",taskDeviceTime,taskDeviceTimeMax);
  display.drawString(0, 50, buffer);
#endif
//  snprintf(buffer, sizeof(buffer),"Loop:%04u Max:%u",loopTime,loopTimeMax);
//  display.drawString(0, 00, buffer);
//  snprintf(buffer, sizeof(buffer),"taskDev:%04u Max:%u",taskDeviceTime,taskDeviceTimeMax);
//  display.drawString(0, 10, buffer);

  snprintf(buffer, sizeof(buffer),"Dev0:%04u Max:%u",taskDeviceTime0,taskDeviceTimeMax0);
  display.drawString(0, 20, buffer);
  snprintf(buffer, sizeof(buffer),"Dev1:%04u Max:%u",taskDeviceTime1,taskDeviceTimeMax1);
  display.drawString(0, 30, buffer);
/*
  snprintf(buffer, sizeof(buffer),"Dev2:%04u Max:%u",taskDeviceTime2,taskDeviceTimeMax2);
  display.drawString(0, 40, buffer);
  snprintf(buffer, sizeof(buffer),"Dev3:%04u Max:%u",taskDeviceTime3,taskDeviceTimeMax3);
  display.drawString(0, 50, buffer);
*/
  snprintf(buffer, sizeof(buffer),"exec:%04u Max:%u",timerExecTime,timerExecTimeMax);
  display.drawString(0, 40, buffer);
  snprintf(buffer, sizeof(buffer),"scan:%04u Max:%u",timerScanTime,timerScanTimeMax);
  display.drawString(0, 50, buffer);

  display.display();

  return;
}

// Input Terminal Man
extern TaskHandle_t taskHandle;
unsigned char sw_list[] = { BUTTON_0, BUTTON_1, BUTTON_2 , BUTTON_3};
InputTerminal tm(sw_list, sizeof(sw_list));

uint8_t itmMan(void){
  uint8_t ret = 0;
  String status;
  unsigned int shortonw;
  unsigned int longonw;
  uint8_t keyData;    // SoundTaskへのタスク間通信データ

  tm.scan();                  // Update the terminaldata
  shortonw = tm.read_s();     // Read the short key input data
  longonw = tm.read_l();      // Read the long key input data

  status = "";
  if (shortonw != 0) {
    ret = shortonw;
    switch (shortonw) {
      case 0x01:
        status = "Short ON : BUTTON_0";
        confWiFiStartf = true;  // WiFi手動接続要求
        break;
      case 0x02:
        status = "Short ON : BUTTON_1";
        loopTimeMax = 0;
        taskDeviceTimeMax = 0;
        taskDeviceTimeMax0 = 0;
        taskDeviceTimeMax1 = 0;
        taskDeviceTimeMax2 = 0;
        taskDeviceTimeMax3 = 0;

//        portENTER_CRITICAL_ISR(&timerMux);
        timerScanTimeMax = 0;
        timerExecTimeMax = 0;
//        portEXIT_CRITICAL_ISR(&timerMux);

        break;
      case 0x03:
        status = "Short ON : BUTTON_0 & BUTTON_1";
        break;
      case 0x04:
        status = "Short ON : BUTTON_2";
        if(uxQueueSpacesAvailable(xQueueSoundPlay) != 0){             // キューの追加可能数が0ではない
          keyData = 2;
//          xQueueSend(xQueueSoundPlay, &keyData, 0);
        }
        break;
      case 0x05:
        status = "Short ON : BUTTON_0 & BUTTON_2";
        break;
      case 0x06:
        status = "Short ON : BUTTON_1 & BUTTON_2";
        break;
      case 0x07:
        status = "Short ON : BUTTON_0 & BUTTON_1 & BUTTON_2";
        break;
      case 0x08:
        status = "Short ON : BUTTON_3";
//        xTaskNotify(sountaskHandle, 0, eIncrement);

        if(uxQueueSpacesAvailable(xQueueSoundPlay) != 0){             // キューの追加可能数が0ではない
          keyData = 1;
//          xQueueSend(xQueueSoundPlay, &keyData, 0);
        }

        break;
      case 0x09:
        status = "Short ON : BUTTON_0 & BUTTON_3";
        break;
      case 0x0A:
        status = "Short ON : BUTTON_1 & BUTTON_3";
        break;
      case 0x0B:
        status = "Short ON : BUTTON_0 & BUTTON_1 & BUTTON_3";
        break;
      case 0x0C:
        status = "Short ON : BUTTON_2 & BUTTON_3";
        break;
      case 0x0D:
        status = "Short ON : BUTTON_0 & BUTTON_2 & BUTTON_3";
        break;
      case 0x0E:
        status = "Short ON : BUTTON_1 & BUTTON_2 & BUTTON_3";
        break;
      case 0x0F:
        status = "Short ON : BUTTON_0 & BUTTON_1 & BUTTON_2 & BUTTON_3";
        break;
      default:
        status = "Short Other Key.";
        break;
    }
    Serial.println(status);
  }
  else if (longonw != 0) {
    ret = longonw + 0x80;
    switch (longonw) {
      case 0x01:
        status = "Long ON : BUTTON_0";
        break;
      case 0x02:
        status = "Long ON : BUTTON_1";
//        dispVFD.dispModeSet(kEY_SET_L);
        break;
      case 0x03:
        status = "Long ON : BUTTON_0 & BUTTON_1";
        break;
      case 0x04:
        status = "Long ON : BUTTON_2";
        if(vfdevent.eventLogSendSqf == EVENT_LOGSEND_WAIT){
          vfdevent.eventLogSendSqf = EVENT_LOGSEND_E2ROM_PRE;  // イベントLOG送信要求 EEPROMログ送信準備
        }
        else{
          Serial.println("-- Log Output Stop. --");
          vfdevent.eventLogSendSqf = EVENT_LOGSEND_WAIT;
        }
        break;
      case 0x05:
        status = "Long ON : BUTTON_0 & BUTTON_2";
        break;
      case 0x06:
        status = "Long ON : BUTTON_1 & BUTTON_2";
        break;
      case 0x07:
        status = "Long ON : BUTTON_0 & BUTTON_1 & BUTTON_2";
        break;
      case 0x08:
        status = "Long ON : BUTTON_3";
        break;
      case 0x09:
        status = "Long ON : BUTTON_0 & BUTTON_3";
        break;
      case 0x0A:
        status = "Long ON : BUTTON_1 & BUTTON_3";
        break;
      case 0x0B:
        status = "Long ON : BUTTON_0 & BUTTON_1 & BUTTON_3";
        break;
      case 0x0C:
        status = "Long ON : BUTTON_2 & BUTTON_3";
        break;
      case 0x0D:
        status = "Long ON : BUTTON_0 & BUTTON_2 & BUTTON_3";
        break;
      case 0x0E:
        status = "Long ON : BUTTON_1 & BUTTON_2 & BUTTON_3";
        break;
      case 0x0F:
        status = "Long ON : BUTTON_0 & BUTTON_1 & BUTTON_2 & BUTTON_3";
        break;
      default:
        status = "Long ON : Other Key";
        break;
    }
    Serial.println(status);
  }
  else {}

  return ret;
}

//-----
// I2C Device serch
//-----
#ifdef DELETE
void DeviceChk::init(void){
  String status;
  datRtc = false;
  datE2ROM = false;

#ifdef USE_I2C

//    Wire.begin(SDA_PIN,SCL_PIN);
    Serial.println("--- DeviceChk init ---");
    getBoardName();

    status = "";
    if(wireChk(I2C_ADDRESS_DS1307) == 0){
        datRtc = true;
        status = status + "I2C_ADDRESS_DS1307 : True!\n";
    }
    else{
        datRtc = false;
        status = status + "I2C_ADDRESS_DS1307 : False!\n";
    }
    if(wireChk(I2C_ADDRESS_24C32) == 0){
        datE2ROM = true;
        status = status + "I2C_ADDRESS_24C32 : True!\n";
    }
    else{
        datE2ROM = false;
        status = status + "I2C_ADDRESS_24C32 : False!\n";
    }

    if(wireChk(I2C_ADDRESS_HDC1000) == 0){
        datHDC1000 = true;
        status = status + "I2C_ADDRESS_HDC1000 : True!\n";
    }
    else{
        datHDC1000 = false;
        status = status + "I2C_ADDRESS_HDC1000 : False!\n";
    }

    if(wireChk(I2C_ADDRESS_SHT30) == 0){
        datSHT30 = true;
        status = status + "I2C_ADDRESS_SHT30 : True!\n";
    }
    else{
        datSHT30 = false;
        status = status + "I2C_ADDRESS_SHT30 : False!\n";
    }
    
    if(wireChk(I2C_ADDRESS_QMP6988) == 0){
        datQMP6988 = true;
        status = status + "I2C_ADDRESS_QMP6988 : True!\n";
    }
    else{
        datQMP6988 = false;
        status = status + "I2C_ADDRESS_QMP6988 : False!\n";
    }
/*    
    if(wireChk(I2C_ADDRESS_SSD1306) == 0){
        datSSD1306 = true;
        status = status + "I2C_ADDRESS_SSD1306 : True!\n";
    }
    else{
        datSSD1306 = false;
        status = status + "I2C_ADDRESS_SSD1306 : False!\n";
    }
*/
    Serial.println(status);
#endif

    return;
}
#endif
/*
  Device使用可能フラグ作成
*/
void DeviceChk::init(void){
  String status = "";;

  datRtc = false;
  datE2ROM = false;
  datHDC1000 = false;
  datSHT30 = false;
  datQMP6988 = false;
  datSSD1306 = false;
  datM5OLED = false;

  for (int i = 0; i < i2cDevice.size(); i++) {
/*
    Serial.print("I2C device found at address 0x");
    if (i2cDevice[i]<16)
      Serial.print("0");
    Serial.print(i2cDevice[i],HEX);
    Serial.println("  !");
*/
    if(i2cDevice[i] == I2C_ADDRESS_M5OLED){
      datM5OLED = true;
      status = status + "I2C_ADDRESS_M5OLED : True!\n";
    }
    else if(i2cDevice[i] == I2C_ADDRESS_SSD1306){
      datSSD1306 = true;
      status = status + "I2C_ADDRESS_SSD1306 : True!\n";
    }
    else if(i2cDevice[i] == I2C_ADDRESS_DS1307){
      datRtc = true;
      status = status + "I2C_ADDRESS_DS1307 : True!\n";
    }
    else if(i2cDevice[i] == I2C_ADDRESS_24C32){
      datE2ROM = true;
      status = status + "I2C_ADDRESS_24C32 : True!\n";
   }
    else if(i2cDevice[i] == I2C_ADDRESS_HDC1000){
      datHDC1000 = false;
      status = status + "I2C_ADDRESS_HDC1000 : False!\n";
    }
    else if(i2cDevice[i] == I2C_ADDRESS_SHT30){
      datSHT30 = true;
      status = status + "I2C_ADDRESS_SHT30 : True!\n";
    }
    else if(i2cDevice[i] == I2C_ADDRESS_QMP6988){
      datQMP6988 = true;
      status = status + "I2C_ADDRESS_QMP6988 : True!\n";
    }
  }
  Serial.println(status);

  return;
}


String DeviceChk::getBoardName(void)
{
  // run-time branch : hardware model check
  String boardName;
  const char* name;
  switch (M5.getBoard())
  {
#if defined (CONFIG_IDF_TARGET_ESP32S3)
  case m5::board_t::board_M5StackCoreS3:
    name = "StackCoreS3";
    break;
  case m5::board_t::board_M5StampS3:
    name = "StampS3";
    break;
  case m5::board_t::board_M5AtomS3U:
    name = "ATOMS3U";
    break;
  case m5::board_t::board_M5AtomS3Lite:
    name = "ATOMS3Lite";
    break;
  case m5::board_t::board_M5AtomS3:
    name = "ATOMS3";
    break;
#elif defined (CONFIG_IDF_TARGET_ESP32C3)
  case m5::board_t::board_M5StampC3:
    name = "StampC3";
    break;
  case m5::board_t::board_M5StampC3U:
    name = "StampC3U";
    break;
#else
  case m5::board_t::board_M5Stack:
    name = "Stack";
    break;
  case m5::board_t::board_M5StackCore2:
    name = "StackCore2";
    break;
  case m5::board_t::board_M5StickC:
    name = "StickC";
    break;
  case m5::board_t::board_M5StickCPlus:
    name = "StickCPlus";
    break;
  case m5::board_t::board_M5StackCoreInk:
    name = "CoreInk";
    break;
  case m5::board_t::board_M5Paper:
    name = "Paper";
    break;
  case m5::board_t::board_M5Tough:
    name = "Tough";
    break;
  case m5::board_t::board_M5Station:
    name = "Station";
    break;
  case m5::board_t::board_M5Atom:
    name = "ATOM";
    break;
  case m5::board_t::board_M5AtomPsram:
    name = "ATOM PSRAM";
    break;
  case m5::board_t::board_M5AtomU:
    name = "ATOM U";
    break;
  case m5::board_t::board_M5TimerCam:
    name = "TimerCamera";
    break;
  case m5::board_t::board_M5StampPico:
    name = "StampPico";
    break;
#endif
  default:
    name = "Who am I ?";
    break;
  }
  Serial.print("BoardName:");
  Serial.println(name);

  boardName = name;
//  Serial.println(boardName);

  return boardName;
}

uint8_t DeviceChk::wireChk(uint8_t address){
    uint8_t error;

    error = 1;
//#ifdef ARDUINO_M5Stack_ATOM
#ifdef USE_I2C
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
#endif
//#endif
    return error;
}

uint8_t DeviceChk::rtc(void){
    return datRtc;
}
bool DeviceChk::e2rom(void){
  return datE2ROM;
}
bool DeviceChk::hdc1000(void){
  return datHDC1000;
}
bool DeviceChk::sht30(void){
  return datSHT30;
}
bool DeviceChk::qmp6988(void){
  return datQMP6988;
}
bool DeviceChk::ssd1306(void){
  return datSSD1306;
//  return false;
//  return true;
}
bool DeviceChk::m5oled(void){
  return datM5OLED;
}

void DeviceChk::i2cScan(void){
  byte error, address;
  int nDevices;

  i2cDevice.clear();    // DeviceList 初期化

#ifdef USE_I2C
 
  Serial.println("Scanning...");
 
  nDevices = 0;
//  address = 0x50;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
//    Serial.println(error);

    if (error == 0)
    {
      i2cDevice.push_back(address);   // DeviceList 追加
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
    else{
//      Serial.println(address,HEX);
//      Serial.println(error);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  for (int i = 0; i < i2cDevice.size(); i++) {
    Serial.print("I2C device found at address 0x");
    if (i2cDevice[i]<16)
      Serial.print("0");
    Serial.print(i2cDevice[i],HEX);
    Serial.println("  !");
  }

  init();
#endif

  return;
}


//==========================================
// LED Control
//==========================================
LEDControl::LEDControl(void)
{
  FastLED.addLeds<WS2811, LED_DATA_PIN, GRB>(leds, NUM_LEDS);   // RGB LEDを初期設定
  FastLED.setBrightness(1);                               // 明るさを設定（20以上は熱で壊れる可能性あり。）

  ledMode = LEDMODE_WIFI_NO_CONNECT;
  ledPattern = LEDPATTERN_ALLOFF;
  ledLasttime = millis();
  ledsqf = LED_OFF;
  return;
}

void LEDControl::ledMan(void)
{
  unsigned long timetemp;

  timetemp = millis();
  if(timetemp - ledLasttime >500){   // 500mSecごとに実行
    ledLasttime = timetemp;
    if(ledsqf == LED_OFF){
      ledsqf = LED_ON;
    }
    else if(ledsqf == LED_ON){
      ledsqf = LED_OFF;
    }
    else{
      ledsqf = LED_OFF;
    }
  }

//  makeLedData();

  if(ledPattern == LEDPATTERN_ALLON){
    LEDSetColor(ledColor);
#ifdef ARDUINO_M5Stack_ATOM
#elif ARDUINO_M5Stick_C
    ledOn();
#endif
  }
  else if(ledPattern == LEDPATTERN_ALLOFF){
    LEDSetColor(ledColor);
#ifdef ARDUINO_M5Stack_ATOM
#elif ARDUINO_M5Stick_C
    ledOff();
#endif
  }
  else{
    if(ledsqf == LED_ON){
      LEDSetColor(ledColor);
#ifdef ARDUINO_M5Stack_ATOM
#elif ARDUINO_M5Stick_C
      ledOn();
#endif
     }
    else{
      ledOff();
    }
  }

//  Serial.print("ledMan:");
//  Serial.println(ledMode);
  return;
}

void LEDControl::setLedMode(uint8_t mode)
{
  ledMode = mode;
  makeLedData();

  return;
}
void LEDControl::makeLedData(void)
{
  switch(ledMode){
    case LEDMODE_WIFI_NO_CONNECT: // WiFi接続なし
      ledColor = 0x000000;
      ledPattern = LEDPATTERN_ALLOFF;   // 消灯
      break;
    case LEDMODE_SNTP_STARTUP_CONNECT_START:
      ledColor = 0x008800;
      ledPattern = LEDPATTERN_ONEBEAT;  // 一拍点滅
      break;
    case LEDMODE_SNTP_STARTUP_CONNECT:
      ledColor = 0x00ff00;
      ledPattern = LEDPATTERN_ALLON;    // 常時点灯
      break;
    case LEDMODE_WIFI_MANUAL_CONNECT_START:
    case LEDMODE_WIFI_MANUAL_DISDISCONNECT:
      ledColor = 0x880000;
      ledPattern = LEDPATTERN_ONEBEAT;
      break;
    case LEDMODE_WIFI_MANUAL_CONNECT:
      ledColor = 0xff0000;
      ledPattern = LEDPATTERN_ALLON;
      break;
    case LEDMODE_WIFI_AUTO_CONNECT_START:
      ledColor = 0x000088;
      ledPattern = LEDPATTERN_ONEBEAT;
      break;
    case LEDMODE_WIFI_AUTO_CONNECT:
      ledColor = 0x0000ff;
      ledPattern = LEDPATTERN_ALLON;
      break;
    default:
      ledColor = 0xaaaaaa;
      ledPattern = LEDPATTERN_ALLON;
  }

  return;
}
uint8_t LEDControl::getLedMode(void)       // LED動作モード取得
{
  return ledMode;
}
void LEDControl::incLedMode(void)          // LED動作モード更新
{
  Serial.print("incLedMode:");
  switch(ledMode){
    case LEDMODE_WIFI_NO_CONNECT:             // WiFi接続なし
      Serial.print(":LEDMODE_WIFI_NO_CONNECT");
      break;
    case LEDMODE_SNTP_STARTUP_CONNECT_START:  // 起動時SNTP接続開始~接続完了
      ledMode = LEDMODE_SNTP_STARTUP_CONNECT;  // 起動時SNTP接続中~WiFi切断
      Serial.print(":LEDMODE_SNTP_STARTUP_CONNECT");
      break;
    case LEDMODE_SNTP_STARTUP_CONNECT:        // // 起動時SNTP接続完了・同期中
//     ledMode = LEDMODE_SNTP_SYNC_COMPLETED;  // 起動時SNTP同期完了~WiFi切断
      ledMode = LEDMODE_WIFI_NO_CONNECT;      // WiFi接続なし
      Serial.print(":LEDMODE_SNTP_SYNC_COMPLETED");
      break;
    
    
    case LEDMODE_WIFI_MANUAL_CONNECT_START:   // WiFi手動接続開始~接続完了
      ledMode = LEDMODE_WIFI_MANUAL_CONNECT;  // WiFi手動接続完了~WiFi切断
      Serial.print(":LEDMODE_WIFI_MANUAL_CONNECT");
      break;
    case LEDMODE_WIFI_MANUAL_CONNECT:         // 起動時SNTP同期完了~WiFi切断
//      ledMode = LEDMODE_SNTP_SYNC_COMPLETED;  // 起動時SNTP同期完了~WiFi切断
      Serial.print(":LEDMODE_WIFI_MANUAL_CONNECT++");
      break;
    case LEDMODE_WIFI_AUTO_CONNECT_START:     // 時刻合わせWiFi自動接続~接続完了
      ledMode = LEDMODE_WIFI_AUTO_CONNECT;    // 時刻合わせWiFi接続完了~WiFi切断
      Serial.print(":LEDMODE_WIFI_AUTO_CONNECT");
      break;
    default:
      break;
  }
  Serial.println("");
  makeLedData();


  return;
}
void LEDControl::setLedPattern(uint8_t pattern)
{
  ledPattern = pattern;
  return;
}

void LEDControl::LEDSetColor(CRGB color)
{
  leds[0] = color;                    // LEDを設定
  leds[1] = CRGB::LightSeaGreen;                    // LEDを設定
  FastLED.setBrightness(10);
  FastLED.show();                           // LEDを表示
//  M5.dis.drawpix(0, color);
  if(color != (CRGB)0x000000){
    digitalWrite(GPIO_NUM_10, LOW);
  }
  else{
    digitalWrite(GPIO_NUM_10, HIGH);
  }
  return;
}
#ifdef ARDUINO_M5Stack_ATOM
#elif ARDUINO_M5Stick_C
void LEDControl::ledOn(void)
{
  digitalWrite(GPIO_NUM_10, LOW);
  
  return;
}
#endif

void LEDControl::ledOff(void)
{
  leds[0] = CRGB::Black;                    // LEDを黒に設定
  FastLED.show();                           // LEDを表示（黒なので消灯）
//  M5.dis.drawpix(0, 0x000000);
#ifdef ARDUINO_M5Stack_ATOM
#elif ARDUINO_M5Stick_C
  digitalWrite(GPIO_NUM_10, HIGH);
#endif
  return;
}

//==========================================
// 
//==========================================
//#ifdef ARDUINO_M5Stack_ATOM
Illuminance::Illuminance(void)
{
    return;
}
uint16_t Illuminance::getaddat(void)
{
  return(addatw);
}
uint16_t Illuminance::read(void)
{
  addatw = analogRead(ILLUMI_PIN);      // 電圧入力
    
  return(addatw);
}

void Illuminance::ini(uint8_t adpin)
{
  illumi_Pin = adpin;
  pinMode(illumi_Pin, ANALOG);              // AD入力ピン設定
  analogSetAttenuation(ADC_11db);       // ADコンバータ　入力アッテネーター設定

  addatw = 0;
}
//#endif

//==========================================
// 
//==========================================
Dcdccont::Dcdccont(void)
{
    return;
}
int Dcdccont::getaddat(void)
{
  return(addatw);
}
void Dcdccont::ini(uint8_t fdbpin,uint8_t pwmpin,uint32_t dcdcTargV)
{
  dcdc_fdPin = fdbpin;
  pinMode(dcdc_fdPin, ANALOG);              // AD入力ピン設定
  analogSetAttenuation(ADC_11db);       // ADコンバータ　入力アッテネーター設定

  ledcSetup(0, 12800, 8);               // 使用するタイマーのチャネルと周波数を設定
  ledcAttachPin(pwmpin, 0);             // ledPinをチャネル0へ接続

  // DCDC目標値作成処理初期化
  glowInTheBright = confDat.GetGlowInTheBright();
  dcdcVBright = (int)((dcdcTargV * (uint32_t)glowInTheBright) / 100);
  glowInTheDark = confDat.GetGlowInTheDark();
  dcdcVDark = (int)((dcdcTargV * (uint32_t)glowInTheDark) / 100);
  dcdcV = dcdcVBright;
  Serial.print("Dcdccont::ini : dcdcVBright : ");
  Serial.println(dcdcVBright);
  Serial.print("Dcdccont::ini : dcdcVDark : ");
  Serial.println(dcdcVDark);

//  ledcWrite(0, 20);                   // チャネル0に出力を設定（テスト用）
  addatw = 0;
  dcdc_runningf = ON;
}
// DCDC目標値作成
int Dcdccont::getTarget(uint16_t illumiData)
{
  int target;

  if(glowInTheBright != confDat.GetGlowInTheBrightTmp()){
    glowInTheBright = confDat.GetGlowInTheBrightTmp();
    dcdcVBright = (int)(((uint32_t)DCDC_TargetV * (uint32_t)glowInTheBright) / 100);
    Serial.print("Dcdccont::getTarget:dcdcVBright : ");
    Serial.println(dcdcVBright);
  }
  if(glowInTheDark != confDat.GetGlowInTheDarkTmp()){
    glowInTheDark = confDat.GetGlowInTheDarkTmp();
    dcdcVDark = (int)(((uint32_t)DCDC_TargetV * (uint32_t)glowInTheDark) / 100);
    Serial.print("Dcdccont::getTarget:dcdcVDark : ");
    Serial.println(dcdcVDark);
  }

  // Brightness control by outside light
  if(illumiData < 1000){
    target = dcdcVDark;
  }
  else{
    target = dcdcVBright;
  }

  return target;
}

void Dcdccont::ctr(int setpwmw)
{
//  int val = 0;               // 電圧フィードバック入力
  int pwmw = 0;              // PWM出力設定値
  int set_pwmpw = 0;         // P項
  int pwmidw;                // I項計算用ΔV
  static long pwmil = 0;     // I項Σdv

  if (dcdc_runningf != OFF) {

    addatw = analogRead(dcdc_fdPin);      // 電圧フィードバック入力
    set_pwmpw = setpwmw / DCDC_PWM_PGAIN;              // P計算
    pwmidw = ((setpwmw - addatw) * DCDC_PWM_IGAIN1) / DCDC_PWM_IGAIN2;         // dv計算
    if (pwmil + (long)pwmidw > (DCDC_PWM_IGAEN_MAX * 0x10000)) {
      pwmil = (DCDC_PWM_IGAEN_MAX * 0x10000);          // Σdv上限
    }
    else if (pwmil + (long)pwmidw < (-1 * DCDC_PWM_IGAEN_MAX * 0x10000)) {
      pwmil = (-1 * DCDC_PWM_IGAEN_MAX * 0x10000);     // Σdv下限
    }
    else {
      pwmil = pwmil + (long)pwmidw;                        // Σdv計算
    }
    pwmw = set_pwmpw + (pwmil >> 8);        // PWM = P+I

    //  Serial.print("pwmw_ori:");
    //  Serial.print(pwmw);

    if (pwmw > DCDC_PWM_MAX) {
      pwmw = DCDC_PWM_MAX; // PWM設定値上限
    }
    else if (pwmw < 0) {
      pwmw = 0; // PWM設定値下限
    }
  }
  else {
    pwmw = 0;
  }

//  noInterrupts();                   // 割り込み禁止
//  Timer1.pwm(DCDC_PWM, pwmw);       // PWM出力設定
//  interrupts();                     // 割り込み許可

  ledcWrite(0, pwmw);                   // チャネル0に出力を設定（テスト用）

/*
    Serial.print("\tRead data:");
    Serial.print(addatw);

    Serial.print("\tPWMp:");
    Serial.print(set_pwmpw);

    Serial.print("\tPWMi:");
    Serial.print(pwmil>>8);

    Serial.print("\tpwmidw:");
    Serial.print(pwmidw);

    Serial.print("\tpwmil:");
    Serial.print(pwmil);

    Serial.print("\tpwmw:");
    Serial.print(pwmw);
    Serial.println("\t");
*/
  return;
}

// IR
#ifdef USE_IR
void irDump(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  uint16_t count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  } else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  } else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  } else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  } else if (results->decode_type == RC5X) {
    Serial.print("Decoded RC5X: ");
  } else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  } else if (results->decode_type == RCMM) {
    Serial.print("Decoded RCMM: ");
  } else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  } else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  } else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  } else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  } else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  } else if (results->decode_type == NIKAI) {
    Serial.print("Decoded Nikai: ");
  }
  serialPrintUint64(results->value, 16);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): {");

  for (uint16_t i = 1; i < count; i++) {
    if (i % 100 == 0)
      yield();  // Preemptive yield every 100th entry to feed the WDT.
    if (i & 1) {
      Serial.print(results->rawbuf[i] * kRawTick, DEC);
    } else {
      Serial.print(", ");
      Serial.print((uint32_t) results->rawbuf[i] * kRawTick, DEC);
    }
  }
  Serial.println("};");
}

// BME680

/**
 * @brief BME680の初期化
 * 
 * @return true 初期化成功
 * @return false 初期化失敗
 */
bool bme680ini(void)
{
  bool ret = true;

// BME680 init
  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
//    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

//  bme680Sqf = 0;

  return ret;
}

/**
 * @brief BME680のデータ取得
 * 
 * @param bme680SensorData データ格納要構造体
 * @return true 取得成功
 * @return false 取得失敗
 */
bool bme680Scan(struct bme680Data *bme680SensorData)
{
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return false;
  }
/*
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();
*/
  bme680SensorData->temperature = bme.temperature;
  bme680SensorData->pressure = bme.pressure;
  bme680SensorData->humidity = bme.humidity;
  bme680SensorData->gas_resistance = bme.gas_resistance;
  bme680SensorData->altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  return true;
}
#endif
