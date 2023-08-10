//
// Created by robot1 on 8/5/23.
//

#include "AwaitTimer.h"
#include <chrono>

AwaitTimer::AwaitTimer(uint64_t timeoutMillis){
    this->timeout = timeoutMillis;
}

void AwaitTimer::start() {
    startTime = getSystemMillis();
}

uint64_t AwaitTimer::getSystemMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

bool AwaitTimer::hasWaitingTimeExceeded(){
    diffTime = getSystemMillis()-startTime;
    return diffTime > timeout;
}