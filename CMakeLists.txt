cmake_minimum_required(VERSION 3.15)
project(SpaceCourier VERSION 1.0 LANGUAGES CXX)

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Настройки для macOS
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")

# Пути для Homebrew
set(HOMEBREW_PREFIX "/opt/homebrew")
set(SFML_DIR "${HOMEBREW_PREFIX}")

# Поиск SFML
find_path(SFML_INCLUDE_DIR SFML/Graphics.hpp PATHS "${SFML_DIR}/include")
find_library(SFML_GRAPHICS sfml-graphics PATHS "${SFML_DIR}/lib")
find_library(SFML_WINDOW sfml-window PATHS "${SFML_DIR}/lib") 
find_library(SFML_SYSTEM sfml-system PATHS "${SFML_DIR}/lib")

if(NOT SFML_INCLUDE_DIR OR NOT SFML_GRAPHICS OR NOT SFML_WINDOW OR NOT SFML_SYSTEM)
    message(FATAL_ERROR "SFML not found! Install with: brew install sfml")
endif()

# Все исходные файлы проекта
add_executable(SpaceCourier
    src/main.cpp
    src/InputHandler.cpp
    src/Entity.cpp
    src/Player.cpp
    src/Platform.cpp
    src/Level.cpp
)

# Заголовочные файлы
target_include_directories(SpaceCourier PRIVATE include ${SFML_INCLUDE_DIR})

# Линковка с SFML
target_link_libraries(SpaceCourier ${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM})

# Копирование SFML библиотек для macOS
if(APPLE)
    add_custom_command(TARGET SpaceCourier POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_GRAPHICS}"
        $<TARGET_FILE_DIR:SpaceCourier>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_WINDOW}"
        $<TARGET_FILE_DIR:SpaceCourier>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_SYSTEM}"
        $<TARGET_FILE_DIR:SpaceCourier>
        COMMENT "Copying SFML libraries for macOS"
    )
endif()

# Копирование assets после сборки
add_custom_command(TARGET SpaceCourier POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:SpaceCourier>/assets
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
message(STATUS "   SFML Include: ${SFML_INCLUDE_DIR}")
message(STATUS "   SFML Libraries: ${SFML_GRAPHICS}, ${SFML_WINDOW}, ${SFML_SYSTEM}")
message(STATUS "   Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SpaceCourier")
message(STATUS " ")