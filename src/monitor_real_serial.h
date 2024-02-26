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
    std::string rsv(void) override;
    int send(std::string data) override;
};

#undef GLOBAL
#endif