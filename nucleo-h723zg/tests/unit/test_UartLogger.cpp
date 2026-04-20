#include <gtest/gtest.h>
#include <string>
#include <Transceiver.h>

#include "uart/Uart.h"

// ── Mock ─────────────────────────────────────────────────────────────────────

class MockTransceiver : public Transceiver {
public:
    std::string captured;
    tl::expected<void, TransceiverError> transmit(const char* data, uint16_t len) override {
        captured.append(data, len);
        return {};  // always succeeds in tests
    }
    tl::expected<uint8_t, TransceiverError> receive(char* buf, uint16_t maxLen) override {
        return {}; // always succeeds in tests
    }
    [[nodiscard]] bool isDataAvailable() const override {
        return false; // no data available in tests
    }

};

// ── Test fixture ─────────────────────────────────────────────────────────────
class UartTest : public ::testing::Test {
   protected:
    MockTransceiver mock;
    Uart transmitter{mock};
};

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_F(UartTest, ConstructCorrectTextOutput) {
    transmitter.logf("Hello world, the answer is %d", 42);
    EXPECT_EQ(mock.captured, "Hello world, the answer is 42");
}

TEST_F(UartTest, ConstructCorrectTextOutputFloat) {
    transmitter.logf("Parsed velocity command: left=%.2f, right=%.2f\n",3.3, 3.3);
    EXPECT_EQ(mock.captured, "Parsed velocity command: left=3.30, right=3.30\n");
}
