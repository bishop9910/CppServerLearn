#include <iostream>
#include "logger.hpp"
#include "config.hpp"
#include <exception>
#include <windows.h>

int main(){
  #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
  #endif
  BS_LOGGER::Logger::getInstance().init("server.log", BS_LOGGER::LogLevel::DEBUG);
  auto& logger = BS_LOGGER::Logger::getInstance();
  auto config = BS_CONFIG::ConfigReader::readConfig();

  logger.info("服务器启动成功，监听端口: {}", config.server_port);
  std::this_thread::sleep_for(std::chrono::seconds(2)); 
  return 0;
}