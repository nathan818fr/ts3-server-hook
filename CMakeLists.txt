cmake_minimum_required(VERSION 3.27)

project(ts3-server-hook LANGUAGES CXX)

set(TS3SH_VERSION "automatic" CACHE STRING "String representing the version of the project")
if (TS3SH_VERSION STREQUAL "automatic")
  execute_process(COMMAND ./tools/print-version.sh
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      OUTPUT_VARIABLE TS3SH_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE
      COMMAND_ERROR_IS_FATAL ANY)
endif ()
message(STATUS "ts3-server-hook version: ${TS3SH_VERSION}")

set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_PREFIX_PATH}")
include("${PROJECT_SOURCE_DIR}/cmake/utils.cmake")

# Detect platforms
set(TS3SH_PLATFORMS "")
if (TRUE) # glibc is the only supported platform for now, assume it's always available
  list(APPEND TS3SH_PLATFORMS "glibc")
endif ()
message(STATUS "Platform-specific sources: ${TS3SH_PLATFORMS}")

# Find dependencies
find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(libmem CONFIG REQUIRED)

# Find sources
set(TS3SH_DIR "${PROJECT_SOURCE_DIR}")
set(TS3SH_INCLUDE_DIRS "${TS3SH_DIR}/src")
find_sources(TS3SH "${TS3SH_DIR}/src" "${TS3SH_PLATFORMS}")

# Define target
add_library(ts3-server-hook SHARED "${TS3SH_SOURCES}" "${TS3SH_HEADERS}")
set_target_properties(ts3-server-hook PROPERTIES PREFIX "") # Remove "lib" prefix

target_compile_features(ts3-server-hook PUBLIC cxx_std_17)
target_compile_options(ts3-server-hook PRIVATE "$<$<CONFIG:Debug>:-Wall;-Werror;-Wextra>")
target_compile_options(ts3-server-hook PUBLIC "$<$<CONFIG:Debug>:-ggdb;-O0>")
target_compile_options(ts3-server-hook PUBLIC "$<$<CONFIG:Release>:-O3>")
target_compile_definitions(ts3-server-hook PRIVATE "TS3SH_VERSION=\"${TS3SH_VERSION}\"")

target_include_directories(ts3-server-hook PRIVATE "${TS3SH_INCLUDE_DIRS}")
target_link_libraries(ts3-server-hook PRIVATE dl Threads::Threads asio::asio libmem::libmem)
