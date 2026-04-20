#include "Uart.h"
#include <cstdio>
#include <cstring>

#include "Receiver.h"
#include "Transceiver.h"

static constexpr size_t kMaxMsg = 256;

Uart::Uart(Transceiver & transceiver):m_transceiver(transceiver) {}

void Uart::print(const char* str) const {
    if (str == nullptr) return;
    (void)m_transceiver.transmit(str, static_cast<uint16_t>(strnlen(str, kMaxMsg)));
}

void Uart::println(const char* str) const {
    print(str);
    (void)m_transceiver.transmit("\r\n", 2);
}

void Uart::logf(const char* fmt, ...) const {
    char buf[kMaxMsg];
    va_list args;
    va_start(args, fmt);
    const int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len > 0)
        (void)m_transceiver.transmit(buf, static_cast<uint16_t>(len < static_cast<int>(sizeof(buf)) ? len
                                                                                : sizeof(buf) - 1));
}

Transceiver & Uart::getTransceiver() const {
     return m_transceiver;
}




