# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================


cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
project(UART_Demo C CXX ASM)
pico_sdk_init()
add_executable(uart_demo uart_demo.c inc/ssd1306.c)
target_link_libraries(uart_demo pico_stdlib hardware_uart hardware_pio hardware_i2c)
target_include_directories(uart_demo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${GENERATED_DIR}   # Adiciona pasta de headers gerados
)
pico_enable_stdio_usb(uart_demo 1)
pico_enable_stdio_uart(uart_demo 0)
pico_add_extra_outputs(uart_demo)