# Unit tests — included by the root CMakeLists.txt via add_subdirectory(tests)
# when CMAKE_CROSSCOMPILING is FALSE (Tests-Native profile, no toolchain file).
# FetchContent requires cmake_minimum_required to be called; the root already
# called it so this is a no-op when used as a subdirectory.
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# CMAKE_CURRENT_SOURCE_DIR is tests/ whether used as subdirectory or standalone
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(unit_tests
    unit/test_BlinkyFreeRTOSTask.cpp
    unit/test_UartLogger.cpp
    unit/test_HostCommandInterface.cpp
    unit/test_EncoderMath.cpp
    ${ROOT}/src/BlinkyFreeRTOSTask.cpp
    ${ROOT}/src/gpio/GpioBase.cpp
    ${ROOT}/src/gpio/GpioOutput.cpp
    ${ROOT}/src/uart/Uart.cpp
    ${ROOT}/src/uart/UartTransceiver.cpp
    ${ROOT}/mocks/mock_impl.cpp
        ../src/host_command_handler/ReceiveCommand.cpp
        ../src/host_command_handler/ReceiveCommand.h
        ../src/host_command_handler/VelocityCommand.cpp
        ../src/host_command_handler/VelocityCommand.h
        ../src/host_command_handler/HostCommeds.h
        ../src/uart/Transceiver.h
)

target_include_directories(unit_tests PRIVATE
    ${ROOT}/mocks        # mock headers shadow the real HAL/FreeRTOS headers
    ${ROOT}/src
    ${ROOT}/src/gpio
    ${ROOT}/src/uart
    ${ROOT}/src/host_command_handler
    ${ROOT}/src/utils
)

target_link_libraries(unit_tests PRIVATE GTest::gtest_main tl::expected)

include(GoogleTest)

# ── Integration tests (require real hardware on a serial port) ────────────────
# Run with: ./build-tests/bin/integration_tests --port /dev/ttyACM0 --baud 115200
add_executable(integration_tests
    integration/test_vel_command.cpp
)

target_compile_features(integration_tests PRIVATE cxx_std_20)

target_link_libraries(integration_tests PRIVATE GTest::gtest)

