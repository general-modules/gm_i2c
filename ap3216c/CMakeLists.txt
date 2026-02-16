cmake_minimum_required(VERSION 3.10)

# 定义静态库
add_library(ap3216c STATIC ap3216c.c)

# 添加头文件搜索路径
target_include_directories(ap3216c PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 添加链接库
target_link_libraries(ap3216c gm_i2c)
