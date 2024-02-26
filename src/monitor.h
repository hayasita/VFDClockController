#ifndef monitor_h
#define monitor_h

#include "monitor_serial.h"

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <cstdint>
#include <vector>
#include <string>

class SerialMonitor{
  private:
    SerialMonitorIO *monitorIo_ = nullptr;

  public:
    /**
     * @brief Construct a new Serial Monitor object
     * 
     * @param pSerialMonitorIo 処理で使用するSerialMonitorIOのポインタを渡す。
     * テストのときはmockを渡す。
     * 実処理の場合は、実処理の派生classのポインタを渡す。
     */
    SerialMonitor(SerialMonitorIO* pSerialMonitorIo){
      monitorIo_ = pSerialMonitorIo;
    }

//    std::vector<std::string> command;

    std::string command();

    
};

#undef GLOBAL
#endif