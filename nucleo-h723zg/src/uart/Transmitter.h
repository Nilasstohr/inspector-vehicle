//
// Created by robotcentral on 4/22/26.
//

#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include <cstdint>
#include <tl/expected.hpp>
#include "TransceiverError.h"

class Transmitter {
public:
    virtual ~Transmitter() = default;
    virtual tl::expected<void, TransceiverError> transmit(const char* data, uint16_t len) const = 0;
};
#endif //TRANSMITTER_H
