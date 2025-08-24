#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>
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
    WINDOW_CREATION_ERROR = 1,
    COULD_NOT_OPEN_FILE_ERROR = 2,
    FORMAT_ERROR = 3
  };

  std::vector<std::string> errorMessages = {
    "CODE 0 | Unknown error",
    "CODE 1 | Window creation error",
    "CODE 2 | Could not open file error",
    "CODE 3 | Format error"
  };

  explicit ErrorHandler(const LogLevel logLevel = ALL) : logLevel(logLevel) {}
  ~ErrorHandler() = default;

  ErrorHandler(const ErrorHandler&) = delete;
  ErrorHandler& operator=(const ErrorHandler&) = delete;

  ErrorHandler(ErrorHandler&&) = default;
  ErrorHandler& operator=(ErrorHandler&&) = default;

  void logFatal(const std::string &message, const ErrorCode errorCode = UNKNOWN_ERROR) const {
    // Every fatal error should be logged
    std::cerr << "[FATAL] (" << errorMessages[errorCode] << "): " << message << std::endl;
    std::exit(EXIT_FAILURE);
  }
  void logError(const std::string &message, const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!(logLevel & ERROR)) return;
    std::cerr << "[ERROR] (" << errorMessages[errorCode] << "): " << message << std::endl;
  }
  void logWarning(const std::string &message, const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!(logLevel & WARNING)) return;
    std::cerr << "[WARNING] (" << errorMessages[errorCode] << "): " << message << std::endl;
  }
  void logInfo(const std::string &message, const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!(logLevel & INFO)) return;
    std::cout << "[INFO] (" << errorMessages[errorCode] << "): " << message << std::endl;
  }

private:
  LogLevel logLevel;
};


#endif // ERROR_HANDLER_H