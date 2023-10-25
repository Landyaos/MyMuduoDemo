cmake_minimum_required(VERSION 3.22)

# 设置 Project
project(demo)

# 设置 C C++标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# 调试使用Debug，可以查看中间变量；发布使用Release，运行速度快
set(CMAKE_BUILD_TYPE Debug)  

# 设置变量，方便后面自动配置，进入子模块的cmake时不用再次设置
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)  # 设置可执行文件的输出目录
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)	   # 设置库文件的输出目录

# add_compile_options(-Wall -Wextra -pedantic -Werror)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++11")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加源文件目录
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRCS)

# 添加测试子模块
enable_testing()
add_subdirectory(test)

# 可执行文件
add_executable(${PROJECT_NAME} ${SRCS})