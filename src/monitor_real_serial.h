/**
 * @file monitor_real_serial.h
 * @author hayasita04@gmail.com
 * @brief シリアルモニタ用送受信実処理　ヘッダ
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef monitor_real_serial_h
#define monitor_real_serial_h

#include "monitor.h"

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <cstdint>
#include <vector>
#include <string>


class RealSerialMonitorIO : public SerialMonitorIO {
  public:
    RealSerialMonitorIO();
    std::string rsv(void) override;
    uint8_t send(std::string data) override;

  private:
    std::string buffer;
};

#undef GLOBAL
#endif