#ifndef TICKER_H
#define TICKER_H

#include "../error_handler/error_handler.h"

class Ticker {
public:
    explicit Ticker(ErrorHandler* errorHandler) : errorHandler(errorHandler) {}
    ~Ticker() = default;

private:
    ErrorHandler* errorHandler;
    unsigned long tick = 0; // Current tick
};

#endif //TICKER_H