#ifndef TICKER_H
#define TICKER_H

#include "../error_handler/error_handler.h"

class Ticker {
public:
    explicit Ticker(ErrorHandler* errorHandler) : errorHandler(errorHandler) {}
    ~Ticker() = default;

    void tick() {
        tickCounter++;
        errorHandler->logInfo("Tick");
    }
    [[nodiscard]] unsigned long getTick() const { return tickCounter; }

private:
    ErrorHandler* errorHandler;
    unsigned long tickCounter = 0; // Current tick
};

#endif //TICKER_H