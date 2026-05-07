#pragma once

#include <chrono>
#include <stdexcept>
#include <string>

// POSIX serial
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/**
 * RAII wrapper around a POSIX serial file-descriptor.
 * Configures 8-N-1, raw mode, with a per-character read timeout.
 *
 * Public API is intentionally minimal: use sendRequest() to send a command
 * and receive the response.  The method blocks until "ack\n" is seen in the
 * incoming data (or the timeout expires), strips "ack\n" from the result, and
 * returns whatever additional data preceded it.
 * Throws std::runtime_error if "ack\n" is not received within the timeout.
 */
class SerialInterface2{
public:
    explicit SerialInterface2(const std::string& path, int baud, int timeoutDs = 20 /*deciseconds*/);
    ~SerialInterface2();

    SerialInterface2(const SerialInterface2&)            = delete;
    SerialInterface2& operator=(const SerialInterface2&) = delete;

    /** Explicitly close the serial port. Safe to call multiple times. */
    void close() noexcept;

    /**
     * Send @p data, wait up to @p timeout for "ack\n", strip it, and store
     * any remaining response bytes.  Retrieve the result with getResponse().
     * @throws std::runtime_error if "ack\n" is not received in time.
     */
    void sendRequest(const std::string& data,
                     std::chrono::milliseconds timeout = std::chrono::milliseconds{200});

    /** Returns the response from the last successful sendRequest() call,
     *  with "ack\n" already stripped. */
    [[nodiscard]] const std::string& getResponse() const noexcept { return m_response; }

private:
    void write(const std::string& data) const;

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

    int m_fd = -1;
    std::string m_response;

    static constexpr const char* k_ack = "ack\n";
};
