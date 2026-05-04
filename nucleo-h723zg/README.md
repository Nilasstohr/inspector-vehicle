# STM32H723ZG — VS Code + FreeRTOS Project

> **Board:** NUCLEO-H723ZG (MB1364)  
> **MCU:** STM32H723ZG — Cortex-M7 @ 64 MHz (HSI, no PLL)  
> **OS:** Ubuntu 22.04  
> **Toolchain:** arm-none-eabi-gcc + OpenOCD + CMake  
> **Language:** C++17 (application) + C11 (HAL/FreeRTOS — handled automatically)

---

## Table of Contents

1. [Prerequisites](#1-prerequisites)
2. [Project Structure](#2-project-structure)
3. [File Ownership — What You Edit vs What You Don't](#3-file-ownership)
4. [Dependencies — What They Are & How to Update](#4-dependencies)
5. [CMake Build System](#5-cmake-build-system)
6. [Linker Script](#6-linker-script)
7. [FreeRTOS Configuration](#7-freertos-configuration)
8. [HAL Configuration](#8-hal-configuration)
9. [Interrupt Handlers — The C++ Gotcha](#9-interrupt-handlers--the-c-gotcha)
10. [VS Code Setup](#10-vs-code-setup)
11. [Build, Flash & Debug Workflow](#11-build-flash--debug-workflow)
12. [Adding New Source Files](#12-adding-new-source-files)
13. [Lessons Learned & Known Pitfalls](#13-lessons-learned--known-pitfalls)

---

## 1. Prerequisites

### One-time installation (Ubuntu)

```bash
# ARM cross-compiler, debugger, flash tool
sudo apt install gcc-arm-none-eabi gdb-multiarch openocd

# ST-Link tools (optional but useful for firmware info)
sudo apt install stlink-tools

# CMake and build tools
sudo apt install cmake ninja-build
```

### ST-Link udev rules (allows flashing without sudo)

```bash
sudo tee /etc/udev/rules.d/49-stlinkv3.rules << 'EOF'
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374f", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3753", MODE="0666", GROUP="plugdev"
EOF
sudo udevadm control --reload-rules && sudo udevadm trigger
sudo usermod -aG plugdev $USER
# Log out and back in for group change to take effect
```

### VS Code extensions

```bash
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension marus25.cortex-debug
code --install-extension github.copilot
code --install-extension github.copilot-chat
code --install-extension twxs.cmake
code --install-extension ms-vscode.vscode-serial-monitor
```

---

## 2. Project Structure

```
nucleo-h723zg/
│
├── CMakeLists.txt              ← Main build file (see Section 5)
├── STM32H723ZGTx_FLASH.ld      ← Linker script — memory map (see Section 6)
├── README.md                   ← This file
│
├── cmake/
│   └── stm32_toolchain.cmake   ← Tells CMake to use arm-none-eabi-gcc
│
├── src/                        ← ✅ YOUR CODE LIVES HERE
│   ├── main.cpp                ← Entry point, task creation, ISR handlers
│   ├── BlinkyFreeRTOSTask.h    ← Example C++ task class (header)
│   ├── BlinkyFreeRTOSTask.cpp  ← Example C++ task class (implementation)
│   ├── FreeRTOSConfig.h        ← FreeRTOS tuning (heap size, priorities, etc.)
│   └── stm32h7xx_hal_conf.h    ← HAL module enable/disable switches
│
├── include/                    ← Put shared headers here
├── tests/unit/                 ← Host-compiled unit tests (Google Test)
├── mocks/                      ← Mock hardware interfaces for testing
│
└── .vscode/
    ├── launch.json             ← Debug configuration (F5)
    ├── tasks.json              ← Build/Flash keyboard shortcuts
    ├── extensions.json         ← Recommended extensions
    └── c_cpp_properties.json   ← IntelliSense configuration
```

---

## 3. File Ownership

### ✅ Files YOU maintain (edit freely)

| File | Purpose |
|------|---------|
| `src/main.cpp` | Entry point — init, task creation, ISR hooks |
| `src/*.cpp / *.h` | Your application code |
| `src/FreeRTOSConfig.h` | FreeRTOS settings (heap, priorities, tick rate) |
| `src/stm32h7xx_hal_conf.h` | Enable/disable HAL modules |
| `CMakeLists.txt` | Add new source files here |
| `STM32H723ZGTx_FLASH.ld` | Memory layout (rarely needs changing) |

### ⚠️ Files managed automatically (do not edit)

| Location | What it is |
|----------|-----------|
| `build/_deps/freertos_kernel-src/` | FreeRTOS source — downloaded by CMake |
| `build/_deps/hal_h7-src/` | STM32H7 HAL — downloaded by CMake |
| `build/_deps/cmsis_device_h7-src/` | ST CMSIS device headers + startup file |
| `build/_deps/cmsis_core-src/` | ARM CMSIS-Core headers (core_cm7.h etc.) |

---

## 4. Dependencies

All dependencies are downloaded automatically by CMake on first configure.  
They are cached in `build/_deps/` — no re-download unless you delete `build/`.

| Dependency | Version | Source | Purpose |
|------------|---------|--------|---------|
| **FreeRTOS-Kernel** | V11.1.0 | GitHub zip | RTOS — tasks, queues, semaphores |
| **STM32H7 HAL** | v1.11.3 | GitHub zip | Hardware abstraction (GPIO, RCC, etc.) |
| **cmsis_device_h7** | v1.10.3 | GitHub zip | STM32H7 register definitions + startup |
| **CMSIS_5 (Core)** | 5.9.0 | GitHub zip | ARM Cortex-M7 core headers |

### How to update a dependency

Edit the `URL` line in `CMakeLists.txt`:

```cmake
# Example: update FreeRTOS to a newer version
FetchContent_Declare(freertos_kernel
    URL https://github.com/FreeRTOS/FreeRTOS-Kernel/archive/refs/tags/V11.2.0.zip
)
```

Then delete the cached download and reconfigure:

```bash
rm -rf build/_deps/freertos_kernel-*
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/stm32_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
```

### Where to find latest versions

| Dependency | Latest releases page |
|------------|---------------------|
| FreeRTOS-Kernel | https://github.com/FreeRTOS/FreeRTOS-Kernel/releases |
| STM32H7 HAL | https://github.com/STMicroelectronics/stm32h7xx_hal_driver/tags |
| cmsis_device_h7 | https://github.com/STMicroelectronics/cmsis_device_h7/tags |
| CMSIS_5 | https://github.com/ARM-software/CMSIS_5/releases |

---

## 5. CMake Build System

### cmake/stm32_toolchain.cmake

Tells CMake to use the ARM cross-compiler instead of the host compiler.

```
Key settings:
  CMAKE_SYSTEM_NAME    = Generic        (no OS, bare metal)
  CMAKE_SYSTEM_PROCESSOR = arm
  CMAKE_C_COMPILER     = arm-none-eabi-gcc
  CMAKE_CXX_COMPILER   = arm-none-eabi-g++
  MCU_FLAGS            = -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
```

The `-mfloat-abi=hard` flag enables hardware floating point — the H723ZG has
a double-precision FPU, making float/double operations fast.

### CMakeLists.txt structure

```
cmake_minimum_required + project declaration
        ↓
FetchContent — downloads 4 zip archives (once, cached in build/_deps/)
        ↓
add_executable — lists ALL .c/.cpp source files to compile
        ↓
target_include_directories — header search paths
        ↓
target_compile_definitions — STM32H723xx, USE_HAL_DRIVER
        ↓
target_link_libraries — links FreeRTOS
        ↓
target_link_options — applies the linker script (.ld file)
        ↓
Post-build — generates .hex and .bin from .elf
```

### Adding a new .cpp file

1. Create `src/MyNewFile.cpp`
2. Add it to `CMakeLists.txt`:

```cmake
add_executable(${PROJECT_NAME}.elf
    src/main.cpp
    src/BlinkyFreeRTOSTask.cpp
    src/MyNewFile.cpp          # ← add here
    ...
)
```

---

## 6. Linker Script

`STM32H723ZGTx_FLASH.ld` tells the linker where to place code and data in memory.

### STM32H723ZG Memory Map

```
┌─────────────────────────────────────────────────────────────┐
│ FLASH   0x08000000  1024 KB  ← Code, constants, read-only   │
├─────────────────────────────────────────────────────────────┤
│ DTCM    0x20000000   128 KB  ← Stack (CPU-only, fastest RAM) │
│ AXI RAM 0x24000000   320 KB  ← Heap, .data, .bss            │
│ SRAM1   0x30000000    16 KB  ← D2 domain                    │
│ SRAM2   0x30004000    16 KB  ← D2 domain                    │
│ SRAM3   0x30008000    16 KB  ← D2 domain                    │
│ SRAM4   0x38000000    16 KB  ← D3 domain                    │
└─────────────────────────────────────────────────────────────┘
```

### ⚠️ Important H7 RAM rule

**DTCM RAM (0x20000000) is NOT accessible by DMA.**  
- Stack lives in DTCM — fine, CPU only.  
- FreeRTOS heap lives in AXI SRAM (`.bss` section) — fine, DMA accessible.  
- If you use DMA buffers, place them in AXI SRAM, NOT DTCM.

```cpp
// Force a buffer into AXI SRAM (DMA safe):
__attribute__((section(".dma_buffer"))) uint8_t myDmaBuffer[1024];
```

---

## 7. FreeRTOS Configuration

`src/FreeRTOSConfig.h` — the main tuning file for FreeRTOS.

### Key settings to know

| Setting | Current Value | What it controls |
|---------|--------------|-----------------|
| `configCPU_CLOCK_HZ` | 64000000 | Must match actual CPU clock |
| `configTICK_RATE_HZ` | 1000 | 1ms tick — `vTaskDelay(1)` = 1ms |
| `configTOTAL_HEAP_SIZE` | 32768 (32KB) | Total RAM for all tasks + FreeRTOS objects |
| `configMINIMAL_STACK_SIZE` | 256 words | Minimum stack for idle task |
| `configMAX_PRIORITIES` | 8 | Task priorities: 0 (lowest) to 7 (highest) |
| `configCHECK_FOR_STACK_OVERFLOW` | 2 | Catches stack overflows (debug builds) |

### ⚠️ Critical ARM Cortex-M interrupt priority defines

```c
#define configPRIO_BITS                          4    // STM32H7 has 4 priority bits
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY  15   // = lowest priority
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
```

**Rule:** Any ISR that calls FreeRTOS API (e.g. `xQueueSendFromISR`) must have
its NVIC priority set between 5 and 15 (inclusive).  
ISRs with priority 0–4 must NEVER call FreeRTOS API.

### Interrupt handler name remapping

```c
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
```

These map FreeRTOS internal handler names to the ARM vector table names.
**Never remove these** or the scheduler will not start.

---

## 8. HAL Configuration

`src/stm32h7xx_hal_conf.h` — controls which HAL modules are compiled.

### Enabling a new peripheral (example: UART)

```c
// Add this line to stm32h7xx_hal_conf.h:
#define HAL_UART_MODULE_ENABLED

// Add the include at the bottom of the file:
#include "stm32h7xx_hal_uart.h"
```

Then add the HAL source to `CMakeLists.txt`:
```cmake
${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_uart.c
${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_uart_ex.c
```

### Currently enabled modules

| Module | Used for |
|--------|---------|
| `HAL_GPIO_MODULE_ENABLED` | LED control, button input |
| `HAL_RCC_MODULE_ENABLED` | Clock configuration |
| `HAL_CORTEX_MODULE_ENABLED` | SysTick, NVIC, MPU |
| `HAL_PWR_MODULE_ENABLED` | Voltage scaling, sleep modes |
| `HAL_FLASH_MODULE_ENABLED` | Required by RCC clock config |

---

## 9. Interrupt Handlers — The C++ Gotcha

**This is the most common mistake when mixing C++ with STM32.**

In a `.cpp` file, C++ applies *name mangling* to function names. The ARM vector
table looks for exact names like `SysTick_Handler`. If the name is mangled,
the CPU falls back to `Default_Handler` (infinite loop).

### ✅ Correct — always wrap ISRs in extern "C"

```cpp
extern "C" {

void SysTick_Handler(void) {
    HAL_IncTick();
    // ...
}

void USART3_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart3);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    while(1);
}

} // extern "C"
```

### ❌ Wrong — will silently fail

```cpp
// In a .cpp file WITHOUT extern "C":
void SysTick_Handler(void) { ... }  // ← mangled name, vector table won't find it!
```

---

## 10. VS Code Setup

### Keyboard shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+Shift+B` | Build |
| `Ctrl+Shift+F` | Flash (builds first) |
| `Ctrl+Alt+F` | Build & Flash |
| `F5` | Start debugger (builds, flashes, halts at `main`) |
| `F10` | Step Over |
| `F11` | Step Into |
| `Shift+F11` | Step Out |
| `Shift+F5` | Stop debugging |

### IntelliSense

IntelliSense reads `build/compile_commands.json` (generated by CMake).  
If you see red squiggles after adding a file, run **Configure** first:

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/stm32_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
```

Or in VS Code: `Ctrl+Shift+P` → `CMake: Configure`

### Debugging features (Cortex-Debug extension)

When you press `F5`:
- Code halts at the first line of `main()`
- **Variables panel** — inspect local and global variables
- **Call Stack** — see which FreeRTOS task is running
- **RTOS Tasks view** — list all tasks with their stack usage
- **Breakpoints** — click left of any line number
- **Watch expressions** — add any variable to the Watch panel

---

## 11. Build, Flash & Debug Workflow

### First time setup (after cloning or on a new machine)

```bash
# 1. Configure — downloads all dependencies (~50MB, once only)
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/stm32_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug

# 2. Build
cmake --build build --parallel

# 3. Flash (board must be connected via USB)
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
  -c "program build/nucleo-h723zg.elf verify reset exit"
```

### Normal daily workflow

```
Edit code in VS Code
      ↓
Ctrl+Alt+F  (Build & Flash)
      ↓
Watch board / F5 to debug
```

### Check if board is connected

```bash
lsusb | grep "0483"
# Should show: ID 0483:374e STMicroelectronics STLINK-V3
```

---

## 12. Adding New Source Files

### Adding a new C++ class (example: a UART driver)

1. Create `src/UartDriver.h` and `src/UartDriver.cpp`

2. Register in `CMakeLists.txt`:
```cmake
add_executable(${PROJECT_NAME}.elf
    src/main.cpp
    src/BlinkyFreeRTOSTask.cpp
    src/UartDriver.cpp          # ← add here
    ...
)
```

3. Enable the HAL module in `stm32h7xx_hal_conf.h`:
```c
#define HAL_UART_MODULE_ENABLED
```

4. Add the HAL source in `CMakeLists.txt`:
```cmake
${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_uart.c
${hal_h7_SOURCE_DIR}/Src/stm32h7xx_hal_uart_ex.c
```

5. Add `UartDriver.h` include to `main.cpp` and use it.

---

## 13. Lessons Learned & Known Pitfalls

### ❌ Power supply — DO NOT call `HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY)`

The Nucleo-H723ZG uses **direct SMPS** supply internally.  
Calling `PWR_LDO_SUPPLY` causes the CPU to hang waiting for a voltage-ready
flag that never comes — all LEDs freeze ON.

```cpp
// ❌ Wrong for this board:
HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

// ✅ Just skip the supply config entirely at 64MHz HSI:
// The board runs fine on its default startup supply settings.
```

### ❌ Missing SysTick_Handler → LEDs freeze ON after 1ms

The startup file defines all interrupt handlers as weak aliases to
`Default_Handler` (infinite loop). Without a strong `SysTick_Handler`,
the CPU halts on the first 1ms tick.

Always define in `main.cpp` inside `extern "C" {}`:
```cpp
void SysTick_Handler(void) {
    HAL_IncTick();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        xPortSysTickHandler();
}
```

### ❌ Missing SVC/PendSV handler mapping → vTaskStartScheduler() does nothing

FreeRTOS starts the scheduler via an SVC instruction. Without mapping:
```c
// In FreeRTOSConfig.h — NEVER remove these:
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
```
The SVC hits `Default_Handler` and hangs silently.

### ❌ DTCM RAM and DMA

DTCM (0x20000000, 128KB) is the fastest RAM but **the DMA controller cannot access it**.  
If you put a DMA buffer in DTCM, DMA transfers will silently fail or produce garbage.  
Always place DMA buffers in AXI SRAM (0x24000000).

### ✅ FreeRTOS heap is in AXI SRAM

`configTOTAL_HEAP_SIZE` creates a `ucHeap[]` array in `.bss`.  
The linker script places `.bss` in AXI SRAM — so FreeRTOS heap is
DMA-accessible by default. 

---

*Generated: April 2026 — Nucleo-H723ZG bring-up with VS Code + CMake + FreeRTOS*

---
## 14. CLion Setup (New Machine)
The `.idea/` folder is committed to the repo — CLion will pick up all CMake profiles and run configurations automatically. You only need to install the host tools.
### One-time tool install
```bash
sudo apt install gcc-arm-none-eabi gdb-multiarch openocd stlink-tools cmake ninja-build
```
### udev rules (flash without sudo)
```bash
sudo tee /etc/udev/rules.d/49-stlinkv3.rules << 'EOF2'
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374f", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3753", MODE="0666", GROUP="plugdev"
EOF2
sudo udevadm control --reload-rules && sudo udevadm trigger
sudo usermod -aG plugdev $USER   # log out + back in
```
### CMake profiles (auto-loaded from `.idea/cmake.xml`)
| Profile | Purpose |
|---|---|
| **Debug** | Firmware build — uses `cmake/stm32_toolchain.cmake` (arm-none-eabi, auto-detected from PATH) |
| **Tests-Native** | Host unit tests — uses system GCC, no cross-compile |
### Run configurations (auto-loaded from `.idea/runConfigurations/`)
| Configuration | What it does |
|---|---|
| **Flash & Debug (OpenOCD)** | Builds firmware, flashes via STLINK-V3, opens GDB debug session |
| **Flash Only** | Runs the CMake `flash` target — quick deploy without a debug session |
| **Unit Tests** | Builds and runs Google Test unit tests on the host |
### First open
1. Open the `nucleo-h723zg/` folder in CLion.
2. CLion detects both CMake profiles — click **Load CMake Project** if prompted.
3. Wait for the **Debug** profile to configure (downloads ~50 MB of deps on first run).
4. Connect the Nucleo board via USB.
5. Select **Flash & Debug (OpenOCD)** in the run-config dropdown → press the debug button (⇧F9 / green bug icon).
> **Tip:** If the Debug CMake profile shows a red cross, the ARM toolchain is not on PATH.  
> Verify with: `arm-none-eabi-gcc --version`
