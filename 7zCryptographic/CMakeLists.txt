cmake_minimum_required(VERSION 3.3)
project(7zCryptographic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ReadArchiveFile.cpp)
#set(SOURCE_FILES ReadArchiveFile.cpp ReadProperties.h verification.cpp Crc32.h)
#set(SOURCE_FILES draft/test.cpp)
add_executable(7zCryptographic ${SOURCE_FILES})
