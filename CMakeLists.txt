cmake_minimum_required(VERSION 3.1)
project(libaopp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

# Define GNU standard installation directories
include(GNUInstallDirs)

# Pull in googletest
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(cmake/DownloadGTest.cmake)

# Pull in clang-tidy checks
include(cmake/clang-dev-tools.cmake)

set(LIBAOPP_SRCS
    src/event.cpp
    )

add_library(aopp SHARED ${LIBAOPP_SRCS})

target_include_directories(aopp PUBLIC include)

enable_testing()
add_subdirectory(test)
