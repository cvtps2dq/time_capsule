cmake_minimum_required(VERSION 3.20)
project(time_capsule
        LANGUAGES CXX
        VERSION 1.0.0
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Clang as compiler
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")

# C++23 configuration
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# Find nlohmann_json
find_package(nlohmann_json 3.11.2 REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
)

# Header files
file(GLOB_RECURSE HEADERS
        "include/*.h"
)

# Create executable
add_executable(time_capsule
        ${SOURCES}
        ${HEADERS}
)

# Link libraries
target_link_libraries(time_capsule PRIVATE
        Qt6::Widgets
        Qt6::Network
        nlohmann_json::nlohmann_json
)

# Include directories
target_include_directories(time_capsule PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Qt6 setup
qt_standard_project_setup()

# Installation (optional)
install(TARGETS time_capsule
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Package configuration (optional)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/time_capsuleConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)