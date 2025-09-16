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
    DEBUG_MESSAGE = 1,
    WINDOW_CREATION_ERROR = 2,
    COULD_NOT_OPEN_FILE_ERROR = 3,
    FORMAT_ERROR = 4,
    FILE_NOT_SUCCESSFULLY_READ_ERROR = 5,
    SHADER_COMPILATION_ERROR = 6,
    PROGRAM_LINKING_ERROR = 7,
    UNASSIGNED_PROVINCE_CATEGORY = 8,
    NOT_ENOUGH_LINE_ELEMENTS = 9
  };

  const std::array<std::string, 10> errorMessages = {
    "CODE 0 | Unknown error",
    "CODE 1 | Debug message",
    "CODE 2 | Window creation error",
    "CODE 3 | Could not open file error",
    "CODE 4 | Format error",
    "CODE 5 | File not successfully read",
    "CODE 6 | Shader compilation error",
    "CODE 7 | Program linking error",
    "CODE 8 | Unassigned province category",
    "CODE 9 | Not enough line elements"
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
  void logDebug(std::string message = "", const ErrorCode errorCode = DEBUG_MESSAGE) const {
    if (errorCode != DEBUG_MESSAGE) std::cout << "Note: Debug logging function used incorrectly. Proceeding anyways." << std::endl;
    if (!message.empty()) message = ": " + message;
    if (logLevel & LOG_DEBUG) std::cout << "[DEBUG] (" << errorMessages[errorCode] << ")" << message << std::endl;
  }

private:
  LogLevel logLevel;
};


#endif // ERROR_HANDLER_H