cmake_minimum_required(VERSION 2.8)
project(librts)

set(RTS_VERSION_MAJOR 0)
set(RTS_VERSION_MINOR 1)

# Add cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Set base CFLAGS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -Werror \
    -Wno-unused-result -Wno-unused-parameter -Wno-unused-function")

set(HEADERS_DIR include/rts)
set(SOURCE_DIR src)

set(HEADERS
    ${HEADERS_DIR}/rts.h
)

set(SOURCE
    ${SOURCE_DIR}/rts.c
)

add_library(rts ${HEADERS} ${SOURCE})
target_include_directories(rts PUBLIC include)

# Tests
add_subdirectory(test)

add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_MODULE_PATH}/uninstall.cmake"
)
