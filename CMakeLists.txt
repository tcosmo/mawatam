cmake_minimum_required(VERSION 2.8.12)
project(datam)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SOURCES "src/*.cpp")
add_executable(datam ${SOURCES})
target_link_libraries(datam ${CONAN_LIBS})