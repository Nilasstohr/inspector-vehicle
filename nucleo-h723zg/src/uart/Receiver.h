//
// Created by robotcentral on 4/24/26.
//

#ifndef RECEIVER_H
#define RECEIVER_H

#include <tl/expected.hpp>
#include "TransceiverError.h"

class Receiver{
public:
    virtual ~Receiver() = default;
    virtual tl::expected<uint8_t, TransceiverError> receive(char* buf, uint16_t maxLen) = 0;
    virtual bool isDataAvailable() const = 0;
};

#endif //RECEIVER_H
