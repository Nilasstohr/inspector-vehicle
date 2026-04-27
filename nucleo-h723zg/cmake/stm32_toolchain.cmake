set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_program(ARM_CC NAMES arm-none-eabi-gcc REQUIRED)
get_filename_component(ARM_TOOLCHAIN_DIR ${ARM_CC} DIRECTORY)

set(CMAKE_C_COMPILER   ${ARM_TOOLCHAIN_DIR}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${ARM_TOOLCHAIN_DIR}/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${ARM_TOOLCHAIN_DIR}/arm-none-eabi-gcc)
set(CMAKE_OBJCOPY      ${ARM_TOOLCHAIN_DIR}/arm-none-eabi-objcopy)
set(CMAKE_SIZE         ${ARM_TOOLCHAIN_DIR}/arm-none-eabi-size)

# Prevent CMake from testing the compiler (it can't run ARM binaries on host)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# STM32H723ZG: Cortex-M7 @ up to 550MHz, FPU double-precision
set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")

set(CMAKE_C_FLAGS_INIT   "${MCU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${MCU_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${MCU_FLAGS} -specs=nano.specs -specs=nosys.specs")

