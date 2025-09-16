#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <array>
#include <string>
#include <iostream>

// Just a simple error handler that can be set for different warning levels
class ErrorHandler {
public:
  // Using bitfields so we can combine levels if needed
  enum LogLevel {
    LOG_INFO = 0b0001,
    LOG_WARNING = 0b0010,
    LOG_ERROR = 0b0100,
    LOG_DEBUG = 0b1000,
    LOG_ALL = 0b1111
  };

  enum ErrorCode {
    UNKNOWN_ERROR = 0,
    WINDOW_CREATION_ERROR = 1,
    COULD_NOT_OPEN_FILE_ERROR = 2,
    FORMAT_ERROR = 3,
    FILE_NOT_SUCCESSFULLY_READ_ERROR = 4,
    SHADER_COMPILATION_ERROR = 5,
    PROGRAM_LINKING_ERROR = 6,
    UNASSIGNED_PROVINCE_CATEGORY = 7,
    NOT_ENOUGH_LINE_ELEMENTS = 8
  };

  const std::array<std::string, 9> errorMessages = {
    "CODE 0 | Unknown error",
    "CODE 1 | Window creation error",
    "CODE 2 | Could not open file error",
    "CODE 3 | Format error",
    "CODE 4 | File not successfully read",
    "CODE 5 | Shader compilation error",
    "CODE 6 | Program linking error",
    "CODE 7 | Unassigned province category",
    "CODE 8 | Not enough line elements"
  };

  explicit ErrorHandler(const LogLevel logLevel = LOG_ALL) : logLevel(logLevel) {}
  ~ErrorHandler() = default;

  ErrorHandler(const ErrorHandler&) = delete;
  ErrorHandler& operator=(const ErrorHandler&) = delete;

  ErrorHandler(ErrorHandler&&) = default;
  ErrorHandler& operator=(ErrorHandler&&) = default;

  void logFatal(std::string message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    // Every fatal error should be logged
    if (!message.empty()) message = ": " + message;
    std::cerr << "[FATAL] (" << errorMessages[errorCode] << ")" << message << std::endl;
    std::exit(EXIT_FAILURE);
  }
  void logInfo(std::string message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!message.empty()) message = ": " + message;
    if (logLevel & LOG_INFO) std::cout << "[INFO] (" << errorMessages[errorCode] << ")" << message << std::endl;
  }
  void logWarning(std::string message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!message.empty()) message = ": " + message;
    if (logLevel & LOG_WARNING) std::cerr << "[WARNING] (" << errorMessages[errorCode] << ")" << message << std::endl;
  }
  void logError(std::string message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!message.empty()) message = ": " + message;
    if (logLevel & LOG_ERROR) std::cerr << "[ERROR] (" << errorMessages[errorCode] << ")" << message << std::endl;
  }
  void logDebug(std::string message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (!message.empty()) message = ": " + message;
    if (logLevel & LOG_DEBUG) std::cout << "[DEBUG] (" << errorMessages[errorCode] << ")" << message << std::endl;
  }

private:
  LogLevel logLevel;
};


#endif // ERROR_HANDLER_H