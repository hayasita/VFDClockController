
#include "monitor.h"

SerialMonitorIO::~SerialMonitorIO() {}

std::string SerialMonitor::command()
{
  std::string ret;
  ret = monitorIo_->rsv();
  return ret;
}


