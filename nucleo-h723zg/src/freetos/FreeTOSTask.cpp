#include "FreeTOSTask.h"

void FreeTOSTask::start(const UBaseType_t priority, const char * name, const uint16_t stackWords)
{
    xTaskCreate(taskEntry, name, stackWords, this, priority, &m_handle);
}
void FreeTOSTask::taskEntry(void* arg)
{
    /* Recover the object pointer and call the member function */
    static_cast<FreeTOSTask*>(arg)->run();
}