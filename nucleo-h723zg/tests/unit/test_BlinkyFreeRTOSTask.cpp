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

TEST_F(BlinkyTaskTest, ConstructorStoresInterval) {
    BlinkyFreeRTOSTask task(pin, 500);
    EXPECT_EQ(task.getInterval(), 500u);
}

TEST_F(BlinkyTaskTest, SetIntervalUpdatesInterval) {
    BlinkyFreeRTOSTask task(pin, 500);
    task.setInterval(250);
    EXPECT_EQ(task.getInterval(), 250u);
}

TEST_F(BlinkyTaskTest, StartCallsXTaskCreateWithCorrectName) {
    BlinkyFreeRTOSTask task(pin, 500);
    task.start("LED", 2);
    EXPECT_TRUE(g_xTaskCreateCalled);
    EXPECT_STREQ(g_xTaskCreateName, "LED");
}

TEST_F(BlinkyTaskTest, StartCallsXTaskCreateWithCorrectPriority) {
    BlinkyFreeRTOSTask task(pin, 500);
    task.start("LED", 3);
    EXPECT_EQ(g_xTaskCreatePriority, 3u);
}

TEST_F(BlinkyTaskTest, SetIntervalToZeroIsAllowed) {
    BlinkyFreeRTOSTask task(pin, 100);
    task.setInterval(0);
    EXPECT_EQ(task.getInterval(), 0u);
}
