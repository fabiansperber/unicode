cmake_minimum_required(VERSION 3.10)
project(unicode LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(HEADERS
  include/unicode.h
)
set(SOURCE
  src/unicode.cpp
)
set(TEST_SOURCE
  test/unicode.cpp
  test/catch.hpp
)

add_library(unicode ${SOURCE} ${HEADERS})
target_include_directories(unicode PRIVATE include)

add_executable(tests WIN32 ${TEST_SOURCE})
target_link_libraries(tests PRIVATE unicode)

add_test(tests tests)
