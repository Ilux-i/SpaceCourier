cmake_minimum_required(VERSION 3.15)
project(SpaceCourier VERSION 1.0 LANGUAGES CXX)

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Настройки для macOS
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures")
endif()

# Настройки сборки
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Поиск SFML (Homebrew)
find_package(PkgConfig QUIET)

if(PKG_CONFIG_FOUND)
    pkg_check_modules(SFML QUIET sfml-all)
endif()

if(SFML_FOUND)
    message(STATUS "✅ SFML found via pkg-config")
else()
    # Ручной поиск SFML в стандартных путях Homebrew
    set(SFML_ROOT "/opt/homebrew")
    
    find_path(SFML_INCLUDE_DIR
        NAMES SFML/Graphics.hpp
        PATHS ${SFML_ROOT}/include
        NO_DEFAULT_PATH
    )
    
    find_library(SFML_GRAPHICS_LIB
        NAMES sfml-graphics
        PATHS ${SFML_ROOT}/lib
        NO_DEFAULT_PATH
    )
    
    find_library(SFML_WINDOW_LIB
        NAMES sfml-window
        PATHS ${SFML_ROOT}/lib
        NO_DEFAULT_PATH
    )
    
    find_library(SFML_SYSTEM_LIB
        NAMES sfml-system
        PATHS ${SFML_ROOT}/lib
        NO_DEFAULT_PATH
    )
    
    if(SFML_INCLUDE_DIR AND SFML_GRAPHICS_LIB AND SFML_WINDOW_LIB AND SFML_SYSTEM_LIB)
        set(SFML_FOUND TRUE)
        set(SFML_INCLUDE_DIRS ${SFML_INCLUDE_DIR})
        set(SFML_LIBRARIES ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB})
        message(STATUS "✅ SFML found manually in ${SFML_ROOT}")
    else()
        message(FATAL_ERROR "❌ SFML not found! Install with: brew install sfml")
    endif()
endif()

# Включение заголовков
include_directories(include)

# Создание исполняемого файла
add_executable(SpaceCourier
    src/main.cpp
    src/InputHandler.cpp
)

# Линковка с SFML
target_link_libraries(SpaceCourier ${SFML_LIBRARIES})

# Настройки специфичные для macOS
if(APPLE)
    # Копирование DLL (не нужно для macOS, но оставим для совместимости)
    
    # Настройки Info.plist (опционально)
    set_target_properties(SpaceCourier PROPERTIES
        MACOSX_BUNDLE_INFO_STRING "Space Courier - 2D Arcade Game"
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2024 Space Courier. All rights reserved."
    )
endif()

# Копирование assets после сборки
add_custom_command(TARGET SpaceCourier POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    COMMENT "Copying assets to output directory"
)

# Полезные цели
add_custom_target(run
    COMMAND ./SpaceCourier
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    DEPENDS SpaceCourier
    COMMENT "Running Space Courier..."
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files"
)

# Вывод информации о конфигурации
message(STATUS " ")
message(STATUS "🎯 Space Courier Configuration:")
message(STATUS "   Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "   C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "   SFML Include: ${SFML_INCLUDE_DIRS}")
message(STATUS "   SFML Libraries: ${SFML_LIBRARIES}")
message(STATUS "   Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SpaceCourier")
message(STATUS " ")