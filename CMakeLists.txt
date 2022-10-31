cmake_minimum_required(VERSION 3.18)

project(MarketConnectivity)

set(CONAN_DISABLE_CHECK_COMPILER 1)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tests)
