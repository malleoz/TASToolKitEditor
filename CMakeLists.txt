cmake_minimum_required(VERSION 3.1.0)

project(TTKEditor VERSION 1.0.0 LANGUAGES CXX)

if (WIN32)
	set (CMAKE_PREFIX_PATH "C:\\Qt\\5.15.2\\msvc2019_64\\")
endif(WIN32)

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
    TTKMainWindow.cpp
    InputFile.cpp
    InputFileModel.cpp
    InputFileMenu.cpp
    InputTableView.cpp
)

target_include_directories(TTKEditor PUBLIC
    "${Qt5_INCLUDE_DIRS}"
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}"
)

if(WIN32)
	set_target_properties(TTKEditor PROPERTIES
		WIN32_EXECUTABLE TRUE
	)

	if($<CONFIG:Debug>)
		get_target_property(WIDGETDLL Qt5::Widgets IMPORTED_LOCATION_DEBUG)
		get_target_property(WIDGETDLL Qt5::Core IMPORTED_LOCATION_DEBUG)
		get_target_property(WIDGETDLL Qt5::Gui IMPORTED_LOCATION_DEBUG)
		get_target_property(WIDGETDLL Qt5::QWindowsIntegrationPlugin IMPORTED_LOCATION_DEBUG)
	else($<CONFIG:Debug>)
		get_target_property(WIDGETDLL Qt5::Widgets IMPORTED_LOCATION_RELEASE)
		get_target_property(WIDGETDLL Qt5::Core IMPORTED_LOCATION_RELEASE)
		get_target_property(WIDGETDLL Qt5::Gui IMPORTED_LOCATION_RELEASE)
		get_target_property(WIDGETDLL Qt5::QWindowsIntegrationPlugin IMPORTED_LOCATION_RELEASE)
	endif($<CONFIG:Debug>)
	
        #TODO: use different copy solution for QWindows

	add_custom_command(
		TARGET TTKEditor POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:TTKEditor>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:TTKEditor>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:TTKEditor>
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> "$<TARGET_FILE_DIR:TTKEditor>/platforms/qwindows.dll"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsVistaStylePlugin> "$<TARGET_FILE_DIR:TTKEditor>/styles/qwindowsvistastyle.dll"
	)
endif(WIN32)

target_link_libraries(TTKEditor Qt5::Widgets)
target_link_libraries(TTKEditor Qt5::Core)
target_link_libraries(TTKEditor Qt5::Gui)
