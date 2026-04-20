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

// POSIX serial
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <thread>
#include <vector>

// ── Runtime configuration (overridden via --port / --baud CLI flags) ──────────

static std::string g_port = "/dev/ttyACM0";
static int         g_baud = 115200;

// ── Helpers ───────────────────────────────────────────────────────────────────

static speed_t baudToSpeed(int baud)
{
    switch (baud) {
        case 9600:   return B9600;
        case 19200:  return B19200;
        case 38400:  return B38400;
        case 57600:  return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        default:     throw std::invalid_argument("Unsupported baud rate");
    }
}

/**
 * RAII wrapper around a POSIX serial file-descriptor.
 * Configures 8-N-1, raw mode, with a per-character read timeout.
 */
class SerialPort {
public:
    explicit SerialPort(const std::string& path, int baud, int timeoutDs = 20 /*deciseconds*/)
    {
        m_fd = ::open(path.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (m_fd < 0)
            throw std::runtime_error("Cannot open serial port: " + path +
                                     " (" + std::strerror(errno) + ")");

        struct termios tty{};
        if (::tcgetattr(m_fd, &tty) != 0)
            throw std::runtime_error("tcgetattr failed");

        speed_t speed = baudToSpeed(baud);
        ::cfsetospeed(&tty, speed);
        ::cfsetispeed(&tty, speed);

        ::cfmakeraw(&tty);            // 8-N-1, no flow control, raw mode
        tty.c_cc[VMIN]  = 0;          // non-blocking read
        tty.c_cc[VTIME] = timeoutDs;  // total read timeout in 0.1 s units

        if (::tcsetattr(m_fd, TCSANOW, &tty) != 0)
            throw std::runtime_error("tcsetattr failed");

        ::tcflush(m_fd, TCIOFLUSH);
    }

    ~SerialPort() { if (m_fd >= 0) ::close(m_fd); }

    SerialPort(const SerialPort&)            = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    void write(const std::string& data) const
    {
        ssize_t written = ::write(m_fd, data.c_str(), data.size());
        if (written < 0 || static_cast<size_t>(written) != data.size())
            throw std::runtime_error("Serial write failed");
    }

    /**
     * Read bytes until @p needle is found in the accumulated buffer or
     * @p timeout elapses.
     */
    template<class Rep, class Period>
    [[nodiscard]] std::string readUntil(const std::string& needle,
                          std::chrono::duration<Rep, Period> timeout) const
    {
        std::string response;
        char byte;
        const auto deadline = std::chrono::steady_clock::now() + timeout;

        while (std::chrono::steady_clock::now() < deadline) {
            ssize_t n = ::read(m_fd, &byte, 1);
            if (n == 1) {
                response += byte;
                if (response.find(needle) != std::string::npos)
                    break;
            }
        }
        return response;
    }

private:
    int m_fd = -1;
};

// ── Test fixture ──────────────────────────────────────────────────────────────

class VelCommandIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        ASSERT_NO_THROW(m_serial = std::make_unique<SerialPort>(g_port, g_baud))
            << "Failed to open " << g_port << " – is the STM32 connected?";

        // Brief settle time for USB-CDC enumeration / board boot
        usleep(500'000);
    }

    std::unique_ptr<SerialPort> m_serial;
};

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_F(VelCommandIntegrationTest, SendVelCommandReceivesAck)
{
    const std::string command      = "VEL 3.3 3.0;";
    const std::string expected_ack = "ack";

    ASSERT_NO_THROW(m_serial->write(command));

    const std::string response = m_serial->readUntil(expected_ack, std::chrono::seconds(2));

    EXPECT_NE(response.find(expected_ack), std::string::npos)
        << "ACK not received. Full response was: \"" << response << "\"";
}

/**
 * Stress test: send "VEL 3.3 3.0;" at ~70 Hz for 1 minute.
 *
 * Each iteration:
 *  - sends the command
 *  - waits up to 10 ms for "ack"
 *  - records a miss if the ACK did not arrive in time
 *  - sleeps for the remainder of the 15 ms period
 *
 * The test fails if ANY single ACK is missed.
 */
TEST_F(VelCommandIntegrationTest, SendVelAt70HzFor1Minute)
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    constexpr auto TEST_DURATION  = 60s;
    constexpr auto PERIOD         = 15ms;   // ~70 Hz
    constexpr auto ACK_TIMEOUT    = 10ms;
    const std::string command     = "VEL 3.3 3.0;";
    const std::string expected_ack = "ack";

    int total  = 0;
    int missed = 0;

    const auto test_end = steady_clock::now() + TEST_DURATION;

    while (steady_clock::now() < test_end) {
        const auto iter_start = steady_clock::now();

        ASSERT_NO_THROW(m_serial->write(command));

        const auto ack_start  = steady_clock::now();
        const std::string response = m_serial->readUntil(expected_ack, ACK_TIMEOUT);
        const double ack_ms = duration<double, std::milli>(steady_clock::now() - ack_start).count();
        ++total;

        if (response.find(expected_ack) != std::string::npos) {
            std::cout << command << "  ->  " << expected_ack
                      << "  after  " << std::fixed << std::setprecision(2)
                      << ack_ms << " ms\n";
        } else {
            ++missed;
            std::cout << command << "  ->  [NO ACK]  after  "
                      << std::fixed << std::setprecision(2) << ack_ms << " ms\n";
            ADD_FAILURE() << "Iteration " << total
                          << ": ACK not received within " << ACK_TIMEOUT.count()
                          << " ms. Response: \"" << response << "\"";
        }

        // Sleep for the remainder of the 15 ms period
        const auto elapsed = steady_clock::now() - iter_start;
        if (elapsed < PERIOD)
            std::this_thread::sleep_for(PERIOD - elapsed);
    }

    std::cout << "[INFO] Sent " << total << " commands, missed " << missed << " ACKs\n";
    EXPECT_EQ(missed, 0) << "Missed " << missed << " / " << total << " ACKs";
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

