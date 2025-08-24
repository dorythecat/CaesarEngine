#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <iostream>

// Just a simple error handler that can be set for different warning levels
class ErrorHandler {
public:
  // Using bitfields so we can combine levels if needed
  enum LogLevel {
    INFO = 0b001,
    WARNING = 0b010,
    ERROR = 0b100,
    ALL = 0b111
  };

  enum ErrorCode {
    UNKNOWN_ERROR = 0,
    FILE_NOT_FOUND = 1,
    COULD_NOT_OPEN_FILE = 2,
    INVALID_FORMAT = 3
  };

  explicit ErrorHandler(const LogLevel logLevel = ALL) : logLevel(logLevel) {}
  ~ErrorHandler() = default;

  ErrorHandler(const ErrorHandler&) = delete;
  ErrorHandler& operator=(const ErrorHandler&) = delete;

  ErrorHandler(ErrorHandler&&) = default;
  ErrorHandler& operator=(ErrorHandler&&) = default;

  void logFatal(const std::string &message, const ErrorCode errorCode = UNKNOWN_ERROR) const {
    // Every fatal error should be logged
    std::cerr << "[FATAL] (" << errorCode << "): " << message << std::endl;
    std::exit(EXIT_FAILURE);
  }
  void logError(const std::string &message, const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!(logLevel & ERROR)) return;
    std::cerr << "[ERROR] (" << errorCode << "): " << message << std::endl;
  }
  void logWarning(const std::string &message, const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!(logLevel & WARNING)) return;
    std::cerr << "[WARNING] (" << errorCode << "): " << message << std::endl;
  }
  void logInfo(const std::string &message, const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!(logLevel & INFO)) return;
    std::cout << "[INFO] (" << errorCode << "): " << message << std::endl;
  }

private:
  LogLevel logLevel;
};


#endif // ERROR_HANDLER_H