cmake_minimum_required(VERSION 3.0)

project(WEBSERVER)


aux_source_directory(. DIR_SRCS)

add_executable(webserver_cmake ${DIR_SRCS})