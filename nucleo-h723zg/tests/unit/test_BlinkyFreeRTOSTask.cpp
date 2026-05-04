#include <gtest/gtest.h>

#include "BlinkyFreeRTOSTask.h"


// Spy state defined in mocks/mock_impl.cpp
extern bool g_xTaskCreateCalled;
extern const char* g_xTaskCreateName;
extern UBaseType_t g_xTaskCreatePriority;


// ── Test fixture ─────────────────────────────────────────────────────────────
class BlinkyTaskTest : public ::testing::Test {
   protected:
    void SetUp() override {
        g_xTaskCreateCalled = false;
        g_xTaskCreateName = nullptr;
        g_xTaskCreatePriority = 0;
    }

    GPIO_TypeDef port{};          /* fake GPIO port for GpioOutput */
    GpioOutput   pin{&port, 0x0020};  /* construct via mock HAL stubs */
};

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_F(BlinkyTaskTest, StartCallsXTaskCreateWithCorrectName) {
    BlinkyFreeRTOSTask task(pin, 500);
    task.start(2,"LED", 512);
    EXPECT_TRUE(g_xTaskCreateCalled);
    EXPECT_STREQ(g_xTaskCreateName, "LED");
}

TEST_F(BlinkyTaskTest, StartCallsXTaskCreateWithCorrectPriority) {
    BlinkyFreeRTOSTask task(pin, 500);
    task.start(3,"LED", 512);
    EXPECT_EQ(g_xTaskCreatePriority, 3u);
}

