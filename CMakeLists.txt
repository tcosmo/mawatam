cmake_minimum_required(VERSION 2.8.12)
project(datam-sim)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SOURCES "src/*.cpp")
add_executable(datam-sim ${SOURCES})
target_link_libraries(datam-sim ${CONAN_LIBS})