#ifndef TICKER_H
#define TICKER_H

#include <functional>
#include <vector>

#include "../error_handler/error_handler.h"

class Ticker {
public:
    explicit Ticker(ErrorHandler* errorHandler) : errorHandler(errorHandler) {}
    ~Ticker() = default;

    void registerTickCallback(const std::function<void()>& callback) { tickCallbacks.push_back(callback); }
    void tick() {
        tickCounter++;
        errorHandler->logInfo("Tick");
        for (const auto& callback : tickCallbacks) callback();
    }
    [[nodiscard]] unsigned long getTick() const { return tickCounter; }

private:
    ErrorHandler* errorHandler;
    unsigned long tickCounter = 0; // Current tick

    std::vector<std::function<void()>> tickCallbacks; // Functions to call on each tick
};

#endif //TICKER_H