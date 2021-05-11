cmake_minimum_required(VERSION 3.13)
project(mawatam)

project(mawatam LANGUAGES CXX)

set(mawatam_VERSION_MAJOR 0)
set(mawatam_VERSION_MINOR 0)
set(mawatam_VERSION_PATCH 2)
set(mawatam_PROG_NAME "mawatam")
set(mawatam_PROG_NAME_EXEC "mawatam")
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

set(EXECUTABLE_NAME "mawatam")
add_executable(mawatam ${SOURCES})

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

ADD_CUSTOM_TARGET( examples ALL  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/examples ${CMAKE_BINARY_DIR}/examples )
ADD_CUSTOM_TARGET( assets ALL  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets )
ADD_CUSTOM_TARGET( mawatam-tools ALL  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/mawatam-tools ${CMAKE_BINARY_DIR}/mawatam-tools )