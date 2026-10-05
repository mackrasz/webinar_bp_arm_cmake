cmake_minimum_required(VERSION 3.20)

# ============================================================================
#  USTAWIENIA ZMIENNYCH ŚRODOWISKOWYCH
# ============================================================================

# Katalog z kompilatorem arm-none-eabi-gcc
set(TOOLCHAIN_PATH "C:/ST/STM32CubeIDE_2.1.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.14.3.rel1.win32_1.0.100.202602081740/tools/bin")

# Pliki źródłowe (.c, .s)
set(SOURCES
    src/main.c
    src/syscalls.c
    src/sysmem.c
    src/system_stm32f4xx.c
    src/startup_stm32f411xe.s
    hal/src/hal_led.c
)

# Katalogi z plikami nagłówkowymi
set(INCLUDE_DIRS
    inc
    hal/inc
    common/cmsis/CMSIS/Core/Include
    common/cmsis-device-f4/Include
)

# Skrypt linkera
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/src/linker.ld)

# OpenOCD: program, katalog skryptów i konfiguracja programatora
set(OPENOCD         "C:/ST/STM32CubeIDE_2.1.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.openocd.win32_2.4.400.202601091506/tools/bin/openocd.exe")
set(OPENOCD_SCRIPTS "C:/ST/STM32CubeIDE_2.1.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.debug.openocd_2.3.300.202602021527/resources/openocd/st_scripts")
set(OPENOCD_CFG     ${CMAKE_SOURCE_DIR}/debug.cfg)

# ============================================================================
#  TOOLCHAIN - musi być ustawiony przed project()
# ============================================================================

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX "${TOOLCHAIN_PATH}/arm-none-eabi-")

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc.exe)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc.exe)
set(CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}objcopy.exe)
set(CMAKE_SIZE         ${TOOLCHAIN_PREFIX}size.exe)

# Test kompilatora bez linkowania (brak skryptu linkera i syscalls na tym etapie)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


# ============================================================================
#  PROJEKT
# ============================================================================

project(webinar_arm C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXECUTABLE_SUFFIX .elf)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MCU_FLAGS -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

target_compile_definitions(${PROJECT_NAME} PRIVATE STM32F411xE)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${MCU_FLAGS}
    -Wall
    $<$<CONFIG:Debug>:-Og -g3>
    $<$<CONFIG:Release>:-Os>
)

target_link_options(${PROJECT_NAME} PRIVATE
    ${MCU_FLAGS}
    -T${LINKER_SCRIPT}
    --specs=nano.specs
)

# Plik .bin do wgrania na płytkę
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)


# Programowanie procesora: cmake --build build --target flash
add_custom_target(flash
    COMMAND ${OPENOCD} -s ${OPENOCD_SCRIPTS} -f ${OPENOCD_CFG}
            -c "program $<TARGET_FILE:${PROJECT_NAME}> verify reset exit"
    DEPENDS ${PROJECT_NAME}
    USES_TERMINAL
)
