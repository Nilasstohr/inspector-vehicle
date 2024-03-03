//
// Created by robot1 on 8/5/23.
//

#include "AwaitTimer.h"
#include <chrono>
#include <iostream>

AwaitTimer::AwaitTimer(uint64_t timeoutMillis){
    this->timeout = timeoutMillis;
}

void AwaitTimer::setTimeout(uint64_t timeout, TimeUnit unit) {
    switch(unit){
        case seconds:{
            this->timeout = 1000*timeout;
            break;
        }
        default:
            this->timeout = timeout;
    }
}

void AwaitTimer::start() {
    startTime = getSystemMillis();
}

uint64_t AwaitTimer::getSystemMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

bool AwaitTimer::hasWaitingTimeExceeded(){
    diffTime = getElapsedTimeMillis();
    return diffTime > timeout;
}

uint64_t AwaitTimer::getElapsedTimeMillis(){
    return  getSystemMillis()-startTime;
}

bool AwaitTimer::reset() {
    startTime = getSystemMillis();
}

void AwaitTimer::printElapsedTime() {
    std::cout << "Time: " << getElapsedTimeMillis() << " ms" << std::endl;
}

AwaitTimer::AwaitTimer() {

}

