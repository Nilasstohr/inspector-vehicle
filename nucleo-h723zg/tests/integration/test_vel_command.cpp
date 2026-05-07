/**
 * Integration test: Send "VEL 3.3 3.0;" to the STM32 via a serial port and
 * confirm that the firmware replies with "ack".
 *
 * Build & run (native host, not cross-compiled):
 *   cmake --build build-tests && ./build-tests/bin/integration_tests \
 *         --port /dev/ttyACM0 --baud 115200
 *
 * The --port and --baud flags are parsed before GTest consumes argv so they
 * can appear anywhere on the command line.
 */

#include <gtest/gtest.h>

#include "SerialInterface2.h"

#include <unistd.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

// ── Runtime configuration (overridden via --port / --baud CLI flags) ──────────

static std::string g_port = "/dev/ttyACM0";
static int         g_baud = 115200;


// ── Test fixture ──────────────────────────────────────────────────────────────

class VelCommandIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        ASSERT_NO_THROW(m_serial = std::make_unique<SerialInterface2>(g_port, g_baud))
            << "Failed to open " << g_port << " – is the STM32 connected?";

        // Brief settle time for USB-CDC enumeration / board boot
        usleep(500'000);
    }

    void TearDown() override
    {
        if (m_serial) {
            try {
                // Always stop motors at the end of every test
                m_serial->sendRequest("v 0 0\n", std::chrono::milliseconds{500});
                std::cout << "[TearDown] Motors stopped (v 0 0)." << std::endl;
            } catch (const std::runtime_error& e) {
                std::cerr << "[TearDown] WARNING: stop command failed: " << e.what() << std::endl;
            }
            m_serial->close();
            std::cout << "[TearDown] Serial port closed." << std::endl;
        }
    }

    /**
     * Sends @p command repeatedly for @p duration, waiting up to @p ackTimeout
     * per iteration for @p expectedAck.  A random inter-command delay in the
     * range [minDelayMs, maxDelayMs] is applied each iteration.
     * Fails the test if any ACK is missed.
     */
    void runStressTest(const std::string& command,
                       std::chrono::seconds testDuration,
                       std::chrono::milliseconds ackTimeout,
                       int minDelayMs = 10,
                       int maxDelayMs = 20) const {
        using namespace std::chrono;

        std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<int> delay_dist{minDelayMs, maxDelayMs};

        int total  = 0;
        int missed = 0;

        const auto test_end = steady_clock::now() + testDuration;

        while (steady_clock::now() < test_end) {
            const auto iter_start = steady_clock::now();
            ++total;

            const auto req_start = steady_clock::now();
            try {
                m_serial->sendRequest(command, ackTimeout);
                const double rtt_ms =
                    duration_cast<duration<double, std::milli>>(steady_clock::now() - req_start).count();

                std::cout << "Passed: sent '" << command << "'  extra='" << m_serial->getResponse()
                          << "'  rtt=" << std::fixed << std::setprecision(2)
                          << rtt_ms << " ms" << std::endl;
            } catch (const std::runtime_error& e) {
                const double rtt_ms =
                    duration_cast<duration<double, std::milli>>(steady_clock::now() - req_start).count();
                ++missed;
                std::cout << "FAILED: sent '" << command << "'  after "
                          << std::fixed << std::setprecision(2) << rtt_ms << " ms"
                          << "  reason: " << e.what() << std::endl;
                ADD_FAILURE() << "Iteration " << total
                              << ": " << e.what();
            }

            // Sleep for the remainder of the random period
            const auto period  = milliseconds(delay_dist(rng));
            const auto elapsed = steady_clock::now() - iter_start;
            if (elapsed < period)
                std::this_thread::sleep_for(period - elapsed);
        }

        std::cout << "[INFO] Sent " << total << " commands, missed " << missed << " ACKs" << std::endl;
        EXPECT_EQ(missed, 0) << "Missed " << missed << " / " << total << " ACKs";
    }

    std::unique_ptr<SerialInterface2> m_serial;
};

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_F(VelCommandIntegrationTest, SendVelCommandReceivesAck)
{
    const std::string command = "v 3.3 3.0\n";

    ASSERT_NO_THROW(m_serial->sendRequest(command, std::chrono::seconds(2)))
        << "ACK not received for command: " << command;

    std::cout << "ACK received. Extra response: '" << m_serial->getResponse() << "'" << std::endl;
}

TEST_F(VelCommandIntegrationTest, SendResetCommandReceivesAck)
{
    const std::string command = "r\n";

    ASSERT_NO_THROW(m_serial->sendRequest(command, std::chrono::seconds(2)))
        << "ACK not received for command: " << command;

    std::cout << "ACK received. Extra response: '" << m_serial->getResponse() << "'" << std::endl;
}

/**
 * Stress test: send "v 3.3 3.0\n" with a random 10–20 ms inter-command delay
 * for 1 minute and verify every iteration receives "ack".
 */
TEST_F(VelCommandIntegrationTest, SendVelAt70HzFor1Minute)
{
    using namespace std::chrono_literals;
    runStressTest("v 3.3 3.0\n",  60s, 10ms);
}

/**
 * Stress test: send "p\n" with a random 10–20 ms inter-command delay
 * for 1 minute and verify every iteration receives "ack".
 */
TEST_F(VelCommandIntegrationTest, SendReadDistanceCommandAt70HzFor1Minute)
{
    using namespace std::chrono_literals;
    runStressTest("p\n", 60s, 10ms);
}

// ── main — parse custom flags before handing off to GTest ────────────────────

int main(int argc, char** argv)
{
    // Consume --port and --baud before GTest sees argv
    std::vector<char*> remaining;
    remaining.push_back(argv[0]);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            g_port = argv[++i];
        } else if (arg == "--baud" && i + 1 < argc) {
            g_baud = std::stoi(argv[++i]);
        } else {
            remaining.push_back(argv[i]);
        }
    }

    int remaining_argc = static_cast<int>(remaining.size());
    ::testing::InitGoogleTest(&remaining_argc, remaining.data());
    return RUN_ALL_TESTS();
}

