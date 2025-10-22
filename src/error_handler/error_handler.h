#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <array>
#include <string>
#include <iostream>
#include <utility>

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
  ErrorHandler& operator=(ErrorHandler&&) = delete;

  void logFatal(const std::string& message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    log("[FATAL]", message, errorCode, true); // Every fatal error should be logged
    std::exit(errorCode + 1000); // Exit the program after fatal errors
  }
  void logInfo(const std::string& message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (logLevel & LOG_INFO) log("[INFO]", message, errorCode);
  }
  void logWarning(const std::string& message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (logLevel & LOG_WARNING) log("[WARNING]", message, errorCode, true);
  }
  void logError(const std::string& message = "", const ErrorCode errorCode = UNKNOWN_ERROR) const {
    if (logLevel & LOG_ERROR) log("[ERROR]", message, errorCode, true);
  }
  void logDebug(const std::string& message = "") const {
    if (logLevel & LOG_DEBUG) log("[DEBUG]", message, DEBUG_MESSAGE);
  }

private:
  LogLevel logLevel;

  void log(const std::string& prefix, const std::string& message, const ErrorCode errorCode, const bool error = false) const {
    if (error) std::cerr << prefix << " (" << errorMessages[errorCode] << ")" << (message.empty() ? "" : ": ") + message << std::endl;
    else std::cout << prefix << " (" << errorMessages[errorCode] << ")" << (message.empty() ? "" : ": ") + message << std::endl;
  }
};


#endif // ERROR_HANDLER_H
