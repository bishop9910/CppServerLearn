#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <mutex>
#include <queue>
#include <thread>
#include <chrono>
#include <source_location>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <filesystem>

namespace std {
  template <>
  struct formatter<std::filesystem::path> : formatter<string> {
    auto format(const std::filesystem::path& p, format_context& ctx) const {
      return formatter<string>::format(p.string(), ctx);
    }
  };
}

namespace BS_LOGGER {
  enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
  };

  inline std::string levelToString(LogLevel level) {
    switch (level) {
      case LogLevel::DEBUG: return "DEBUG";
      case LogLevel::INFO:  return "INFO ";
      case LogLevel::WARN:  return "WARN ";
      case LogLevel::ERROR: return "ERROR";
      case LogLevel::FATAL: return "FATAL";
      default:              return "UNKNOWN";
    }
  }

  class Logger {
  public:
    static Logger& getInstance() {
      static Logger instance;
      return instance;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void init(const std::string& log_file_path = "server.log", LogLevel min_level = LogLevel::INFO);

    template <typename... Args>
    void log(const std::source_location& location, LogLevel level, 
        std::string_view fmt_str, 
        Args&&... args) {
      
      if (level < m_min_level.load()) return;

      std::string message = std::vformat(fmt_str, std::make_format_args(args...));

      auto now = std::chrono::system_clock::now();
      auto time_t_now = std::chrono::system_clock::to_time_t(now);
      std::tm tm_now;
      #ifdef _WIN32
        localtime_s(&tm_now, &time_t_now);
      #else
        localtime_r(&time_t_now, &tm_now);
      #endif
      char time_buf[64];
      std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm_now);

      std::string log_entry = std::format("[{}] [{}] [{}:{}] {}\n", 
        time_buf, 
        levelToString(level), 
        location.file_name(), 
        location.line(), 
        message);
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(log_entry));
      }
      m_cv.notify_one();
    }

    void debug(std::string_view fmt, auto&&... args) { 
      log(std::source_location::current(), LogLevel::DEBUG, fmt, std::forward<decltype(args)>(args)...); 
    }
    void info(std::string_view fmt, auto&&... args)  { 
      log(std::source_location::current(), LogLevel::INFO, fmt, std::forward<decltype(args)>(args)...); 
    }
    void warn(std::string_view fmt, auto&&... args)  { 
      log(std::source_location::current(), LogLevel::WARN, fmt, std::forward<decltype(args)>(args)...); 
    }
    void error(std::string_view fmt, auto&&... args) { 
      log(std::source_location::current(), LogLevel::ERROR, fmt, std::forward<decltype(args)>(args)...); 
    }
    void fatal(std::string_view fmt, auto&&... args) { 
      log(std::source_location::current(), LogLevel::FATAL, fmt, std::forward<decltype(args)>(args)...); 
    }

    void shutdown();

  private:
    Logger() = default;
    ~Logger() { shutdown(); }

    void workerThreadFunc(std::stop_token stopToken);

    std::ofstream m_file_stream;
    std::queue<std::string> m_queue;
    std::mutex m_mutex;
    std::condition_variable_any m_cv;
    std::jthread m_worker_thread;
    std::atomic<LogLevel> m_min_level{LogLevel::INFO};
    bool m_initialized = false;
  };

} // namespace BS_LOGGER