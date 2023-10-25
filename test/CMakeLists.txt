cmake_minimum_required(VERSION 3.22)

add_executable(TestBuffer buffer/TestBuffer.cc ${PROJECT_SOURCE_DIR}/src/CircularBuffer.cc ${PROJECT_SOURCE_DIR}/src/HttpRequest.cc )

add_test(NAME TestBuffer COMMAND TestBuffer)