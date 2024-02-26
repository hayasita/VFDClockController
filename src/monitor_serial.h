#ifndef monitor_serial_h
#define monitor_serial_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <cstdint>
#include <vector>
#include <string>

class SerialMonitorIO{
  public:
    virtual ~SerialMonitorIO(void) = 0;   // 純粋仮想デストラクタ
    virtual std::string rsv(void) = 0;    // 純粋仮想関数
    virtual int send(std::string data) = 0;           // 純粋仮想関数
};

#undef GLOBAL
#endif