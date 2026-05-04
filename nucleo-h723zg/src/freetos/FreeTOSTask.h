//
// Created by robotcentral on 4/30/26.
//

#ifndef FREETOSTASK_H
#define FREETOSTASK_H

#include "FreeRTOS.h"
#include "task.h"


class FreeTOSTask {
public:
    virtual ~FreeTOSTask() = default;
    explicit FreeTOSTask() = default;
    FreeTOSTask(const FreeTOSTask&)            = delete;
    FreeTOSTask& operator=(const FreeTOSTask&) = delete;
    FreeTOSTask(FreeTOSTask&&)                 = delete;
    FreeTOSTask& operator=(FreeTOSTask&&)      = delete;
    void start(UBaseType_t priority, const char *name, uint16_t stackWords);
protected:
    virtual void run() = 0;
private:
    static void taskEntry(void* arg);
    TaskHandle_t m_handle = nullptr;
};


#endif //FREETOSTASK_H
