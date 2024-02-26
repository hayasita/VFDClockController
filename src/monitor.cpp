
#include "monitor.h"


std::string SerialMonitor::command()
{
  std::string ret;
  ret = monitorIo_->rsv();
  return ret;
}


