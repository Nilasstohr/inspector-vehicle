#include "SerialInterface2.h"

#include <cerrno>
#include <cstring>
#include <stdexcept>

// ── Internal helper ───────────────────────────────────────────────────────────

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

// ── SerialInterface2 ───────────────────────────────────────────────────────────

SerialInterface2::SerialInterface2(const std::string& path, int baud, int timeoutDs)
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

SerialInterface2::~SerialInterface2()
{
    close();
}

void SerialInterface2::close() noexcept
{
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void SerialInterface2::write(const std::string& data) const
{
    ssize_t written = ::write(m_fd, data.c_str(), data.size());
    if (written < 0 || static_cast<size_t>(written) != data.size())
        throw std::runtime_error("Serial write failed");
}

void SerialInterface2::sendRequest(const std::string& data,
                                   std::chrono::milliseconds timeout)
{
    m_response.clear();

    write(data);

    std::string raw = readUntil(k_ack, timeout);

    const auto pos = raw.find(k_ack);
    if (pos == std::string::npos)
        throw std::runtime_error("ACK not received. Response: \"" + raw + "\"");

    raw.erase(pos, std::string{k_ack}.size()); // strip "ack\n"
    m_response = std::move(raw);
}
