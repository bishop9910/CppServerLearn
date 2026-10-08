#include "logger.hpp"
#include <iostream>

void BS_LOGGER::Logger::init(const std::string& log_file_path, LogLevel min_level) {
  if (m_initialized) return;

  m_min_level.store(min_level);

  m_file_stream.open(log_file_path, std::ios::out | std::ios::app);
  if (!m_file_stream.is_open()) {
    std::cerr << "[Logger Error] Failed to open log file: " << log_file_path << std::endl;
  }

  m_initialized = true;

  m_worker_thread = std::jthread([this](std::stop_token st) {
    this->workerThreadFunc(st);
  });
}

void BS_LOGGER::Logger::workerThreadFunc(std::stop_token stopToken) {
while (true) {
  std::string entry;
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, stopToken, [this, &stopToken] { 
      return !m_queue.empty() || stopToken.stop_requested(); 
    });

    if (stopToken.stop_requested() && m_queue.empty()) {
      break;
    }

    if (!m_queue.empty()) {
        entry = std::move(m_queue.front());
        m_queue.pop();
    }
  }

  if (!entry.empty()) {
    std::cout << entry; 
    
    if (m_file_stream.is_open()) {
      m_file_stream << entry;
      m_file_stream.flush();
    }
  }
}
}

void BS_LOGGER::Logger::shutdown() {
  if (!m_initialized) return;
  
  m_worker_thread.request_stop();
  
  m_cv.notify_all();
  
  if (m_file_stream.is_open()) {
    m_file_stream.close();
  }
  m_initialized = false;
}