cmake_minimum_required(VERSION 3.1.0)

project(TTKEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(TTKEditor
    main.cpp
    TASToolKitEditor.cpp
    InputFile.cpp
    InputFileModel.cpp
)

target_include_directories(TTKEditor PUBLIC
    "${Qt5_INCLUDE_DIRS}"
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(TTKEditor Qt5::Widgets)