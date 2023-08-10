//
// Created by robot1 on 8/5/23.
//

#ifndef ROBOTVEHICLE_PACKAGE_AWAITTIMER_H
#define ROBOTVEHICLE_PACKAGE_AWAITTIMER_H


#include <cstdint>

class AwaitTimer {
public:
    AwaitTimer( uint64_t timeoutMillis);
    void start();
    bool hasWaitingTimeExceeded();
private:
    uint64_t timeout;
    uint64_t startTime;
    uint64_t diffTime;
    uint64_t getSystemMillis();
};


#endif //ROBOTVEHICLE_PACKAGE_AWAITTIMER_H
