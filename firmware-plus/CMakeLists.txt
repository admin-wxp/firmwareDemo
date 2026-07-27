# CMakeLists.txt
# 作用：跨平台构建固件库和可执行文件
# 支持编译为静态库/动态库/可执行文件，方便测试调用

cmake_minimum_required(VERSION 3.10)
project(cimfw C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 头文件路径
include_directories(
    ${CMAKE_SOURCE_DIR}/firmware/hal
    ${CMAKE_SOURCE_DIR}/firmware/driver
    ${CMAKE_SOURCE_DIR}/firmware/core
)

# 源文件
set(FW_SOURCES
    firmware/hal/hal_reg.c
    firmware/driver/adc_driver.c
    firmware/driver/flash_ctrl.c
    firmware/core/interrupt.c
)

# 编译为共享库（供 Python ctypes 调用）
add_library(cimfw SHARED ${FW_SOURCES})

# 编译为可执行文件（Bring-up 演示）
add_executable(bringup firmware/main.c)
target_link_libraries(bringup cimfw)