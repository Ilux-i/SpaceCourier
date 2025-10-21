cmake_minimum_required(VERSION 3.15)
project(SpaceCourier VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0")

set(HOMEBREW_PREFIX "/opt/homebrew")
set(SFML_DIR "${HOMEBREW_PREFIX}")

find_path(SFML_INCLUDE_DIR SFML/Graphics.hpp PATHS "${SFML_DIR}/include")
find_library(SFML_GRAPHICS sfml-graphics PATHS "${SFML_DIR}/lib")
find_library(SFML_WINDOW sfml-window PATHS "${SFML_DIR}/lib") 
find_library(SFML_SYSTEM sfml-system PATHS "${SFML_DIR}/lib")

if(NOT SFML_INCLUDE_DIR OR NOT SFML_GRAPHICS OR NOT SFML_WINDOW OR NOT SFML_SYSTEM)
    message(FATAL_ERROR "SFML not found! Install with: brew install sfml")
endif()

add_executable(SpaceCourier
    src/main.cpp
    src/InputHandler.cpp
)

target_include_directories(SpaceCourier PRIVATE include ${SFML_INCLUDE_DIR})

target_link_libraries(SpaceCourier ${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM})

add_custom_command(TARGET SpaceCourier POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:SpaceCourier>/assets
)

message(STATUS "✅ SFML found: ${SFML_DIR}")
message(STATUS "✅ Project configured for macOS")