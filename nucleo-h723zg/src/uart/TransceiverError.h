//
// Created by robotcentral on 4/24/26.
//

#ifndef TRANSCEIVERERROR_H
#define TRANSCEIVERERROR_H
#include <cstdint>

/** Errors that any Transmitter implementation can return. */
enum class TransceiverError : uint8_t {
    Timeout,   ///< Hardware did not accept the data in time
    Busy,      ///< Peripheral busy (mutex timeout or HAL busy)
};


#endif //TRANSCEIVERERROR_H
