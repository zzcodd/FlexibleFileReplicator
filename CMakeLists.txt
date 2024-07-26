# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(myProject VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source files
set(SOURCES
    copy.cc
    log.cc
    main.cc
)

# Add the header files
set(HEADERS
    copy.h
    disk.h
    log.h
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Find threads package
find_package(Threads REQUIRED)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link threads library
target_link_libraries(${PROJECT_NAME} Threads::Threads)

# Specify the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
