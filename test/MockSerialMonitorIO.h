#ifndef MOCK_SERIAL_MONITOR_IO_H_
#define MOCK_SERIAL_MONITOR_IO_H_

#include <gmock/gmock.h>
#include "../src/monitor.h"

class MockSerialMonitorIO : public SerialMonitorIO
{
  public:
    MOCK_METHOD(std::string , rsv, ());
    MOCK_METHOD(int , send, (std::string data));
};

#endif // MOCK_SERIAL_MONITOR_IO_H_