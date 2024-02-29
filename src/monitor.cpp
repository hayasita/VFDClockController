/**
 * @file monitor.cpp
 * @author hayasita04@gmail.com
 * @brief シリアルモニタ処理
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "monitor.h"

/**
 * @brief Destroy the Serial Monitor I O:: Serial Monitor I O object
 * 
 */
SerialMonitorIO::~SerialMonitorIO() {}

/**
 * @brief シリアルモニタ実行
 * 
 * @return true 
 * @return false 
 */
bool SerialMonitor::exec(void)
{
  std::string commandBuf;
  bool ret = true;

  commandBuf = monitorIo_->rsv();

  if(commandBuf.size() > 0){
    commandBuf.push_back('\n');
    monitorIo_->send(commandBuf);
  }

  return ret;
}

