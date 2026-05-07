cmake_minimum_required(VERSION 3.20)

# NOTE: The STM32 toolchain is passed by the CLion STM32-Debug CMake profile via
#   -DCMAKE_TOOLCHAIN_FILE=.../cmake/stm32_toolchain.cmake
# Do NOT hardcode it here — that would break the native Tests-Native profile.

project(nucleo-h723zg C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── tl::expected — available to both firmware and unit tests ─────────────────
# Result<T,E> / std::expected backport (C++14/17). Single-header, no heap, no exceptions.
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# DOWNLOAD_EXTRACT_TIMESTAMP was added in CMake 3.24; guard it so the project
# also configures cleanly on 3.22 (Ubuntu 22.04 ships 3.22).
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
    cmake_policy(SET CMP0135 NEW)   # silence the timestamp warning on 3.24+
endif()

FetchContent_Declare(tl_expected
    URL https://github.com/TartanLlama/expected/archive/refs/tags/v1.1.0.zip
)
FetchContent_MakeAvailable(tl_expected)

# ── ETL (Embedded Template Library) — no heap, no exceptions, C++17 ──────────
FetchContent_Declare(etl
    URL https://github.com/ETLCPP/etl/archive/refs/tags/20.47.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(etl)

# ── Firmware (cross-compile only) ────────────────────────────────────────────
if(CMAKE_CROSSCOMPILING)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)  # Don't re-download on every cmake run

# ── 1. ARM CMSIS-Core headers (core_cm7.h etc.) ──────────────────────────────
FetchContent_Declare(cmsis_core
    URL https://github.com/ARM-software/CMSIS_5/archive/refs/tags/5.9.0.zip
)
FetchContent_MakeAvailable(cmsis_core)

# ── 2. STM32H7 CMSIS Device (stm32h723xx.h, startup, system_stm32h7xx.c) ────
FetchContent_Declare(cmsis_device_h7
    URL https://github.com/STMicroelectronics/cmsis_device_h7/archive/refs/tags/v1.10.3.zip
)
FetchContent_MakeAvailable(cmsis_device_h7)

# ── 3. STM32H7 HAL Driver ────────────────────────────────────────────────────
FetchContent_Declare(hal_h7
    URL https://github.com/STMicroelectronics/stm32h7xx_hal_driver/archive/refs/tags/v1.11.3.zip
)
FetchContent_MakeAvailable(hal_h7)

# ── 4. FreeRTOS Kernel ───────────────────────────────────────────────────────
FetchContent_Declare(freertos_kernel
    URL https://github.com/FreeRTOS/FreeRTOS-Kernel/archive/refs/tags/V11.1.0.zip
)
# FreeRTOS-Kernel requires a 'freertos_config' target that provides FreeRTOSConfig.h
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE ${CMAKE_SOURCE_DIR}/src/freetos)

set(FREERTOS_PORT GCC_ARM_CM7 CACHE STRING "" FORCE)
set(FREERTOS_HEAP 4           CACHE STRING "" FORCE)
FetchContent_MakeAvailable(freertos_kernel)

# ── Application executable ───────────────────────────────────────────────────
add_executable(${PROJECT_NAME}.elf
    # Application (C++)
    src/main.cpp
    src/BlinkyFreeRTOSTask.cpp
    src/CrashHandler.cpp
        src/uart/Uart.cpp

    # STM32H7 HAL (only what blinky needs)
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_rcc.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_rcc_ex.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_gpio.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_cortex.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_pwr.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_pwr_ex.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_flash.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_flash_ex.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_dma.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_uart.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_uart_ex.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_tim.c
    ${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_tim_ex.c

    # CMSIS system init
    ${cmsis_device_h7_SOURCE_DIR}/Source/Templates/system_stm32h7xx.c

    # Startup (assembly) — provided by ST CMSIS device package
    ${cmsis_device_h7_SOURCE_DIR}/Source/Templates/gcc/startup_stm32h723xx.s
        src/uart/Transmitter.h
        src/uart/UartLineReceiver.cpp
        src/uart/UartLineReceiver.h
        src/uart/UartTransceiver.cpp
        src/uart/UartTransceiver.h
        src/host_command_handler/HostCommandHandlerFreeTOSTask.cpp
        src/host_command_handler/HostCommandHandlerFreeTOSTask.h
        src/host_command_handler/ReceiveCommand.cpp
        src/host_command_handler/ReceiveCommand.h
        src/host_command_handler/VelocityCommand.cpp
        src/host_command_handler/VelocityCommand.h
        src/gpio/GpioOutput.cpp
        src/gpio/GpioBase.cpp
        src/gpio/GpioInput.cpp
        src/pwm/PwmOutput.cpp
        src/pwm/PwmOutput.h
        src/timer/HardwareTimer.cpp
        src/timer/HardwareTimer.h
        src/motors/MotorDriver.cpp
        src/motors/MotorDriver.h
        src/motors/Encoder.cpp
        src/motors/Encoder.h
        src/isr_handlers.cpp
        src/freetos/FreeTOSTask.h
        src/freetos/FreeTOSTask.cpp
        src/motors/TelemetryTask.cpp
        src/motors/TelemetryTask.h
        src/motors/PiMotorControl.cpp
        src/motors/PiMotorControl.h
        src/motors/TransposedIIRFilter.cpp
        src/motors/TransposedIIRFilter.h
        src/host_command_handler/DistanceReadRequestCommand.h
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/utils
    ${cmsis_core_SOURCE_DIR}/CMSIS/Core/Include
    ${cmsis_device_h7_SOURCE_DIR}/Include
    ${hal_h7_SOURCE_DIR}/Inc
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    STM32H723xx
    USE_HAL_DRIVER
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -Wall
    -Wextra
    -Wno-missing-field-initializers  # HAL structs zero-initialised with {0}
    -ffunction-sections
    -fdata-sections
    $<$<CONFIG:Debug>:-Og -g3 -gdwarf-2>
    $<$<CONFIG:Release>:-O2>
)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    freertos_kernel
    tl::expected
    etl::etl
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32H723ZGTx_FLASH.ld
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -u _printf_float   # enable %f/%g in newlib-nano vsnprintf
)

# ── Post-build: .hex and .bin ────────────────────────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    VERBATIM
)

# ── Flash target (st-flash via ST-Link) ─────────────────────────────────────
# Usage from terminal:  cmake --build . --target flash
# In CLion: create a "Custom Build Target" → build target = flash
find_program(ST_FLASH st-flash)
find_program(OPENOCD   openocd)

if(ST_FLASH)
    add_custom_target(flash
        DEPENDS ${PROJECT_NAME}.elf
        COMMAND ${ST_FLASH} --reset write ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin 0x08000000
        COMMENT "Flashing via st-flash..."
        VERBATIM
    )
elseif(OPENOCD)
    add_custom_target(flash
        DEPENDS ${PROJECT_NAME}.elf
        COMMAND ${OPENOCD}
            -f interface/stlink.cfg
            -f target/stm32h7x.cfg
            -c "program ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf verify reset exit"
        COMMENT "Flashing via OpenOCD..."
        VERBATIM
    )
else()
    message(STATUS "Neither st-flash nor openocd found — 'flash' target unavailable")
endif()

endif() # CMAKE_CROSSCOMPILING

# ── Unit Tests (host-native build only) ─────────────────────────────────────
if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tests)
endif()
