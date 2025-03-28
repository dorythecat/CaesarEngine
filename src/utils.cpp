#include "utils.hpp"

void printException(const std::exception& e) {
  std::vector<std::string> messages;
  const std::exception* current = &e;

  while (current) {
    messages.push_back(current->what());
    try {
      std::rethrow_if_nested(*current);
    } catch (const std::exception& nested) {
      current = &nested;
    } catch (...) {
      messages.push_back("ERROR::xxxxxx::UNKNOWN ERROR");
      break;
    }
  }

  int level = 0;
  for (const std::string& message : messages) {
    std::cerr << std::string(level, ' ') << "ERROR::" << message << std::endl;
    level++;
  }
}
