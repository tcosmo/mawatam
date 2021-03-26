cmake_minimum_required(VERSION 3.13)
project(datam)

project(datam LANGUAGES CXX)

set(datam_VERSION_MAJOR 0)
set(datam_VERSION_MINOR 0)
set(datam_VERSION_PATCH 1)
set(datam_PROG_NAME "datam")
set(datam_PROG_NAME_EXEC "datam")
configure_file(
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h"
  )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "-O3")

include_directories("${PROJECT_BINARY_DIR}")

file(GLOB SOURCES "src/*.cpp" "src/yaml/*.cpp" "src/easylogging++/*.cpp" )

set(EXECUTABLE_NAME "datam")
add_executable(datam ${SOURCES})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions

find_package(SFML 2 REQUIRED graphics window system)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries( ${EXECUTABLE_NAME}   ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

# CPack packaging
# include(InstallRequiredSystemLibraries)
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
# set(CPACK_PACKAGE_VERSION_MAJOR "${visutiles_VERSION_MAJOR}")
# set(CPACK_PACKAGE_VERSION_MINOR "${visutiles_VERSION_MINOR}")
# include(CPack)