set(TTKE_Headers
    InputFile.h
    InputFileMenu.h
    InputFileModel.h
    InputTableView.h
    TTKMainWindow.h
    Definitions.h
    )

set(TTKE_CPP
    InputFile.cpp
    InputFileMenu.cpp
    InputFileModel.cpp
    InputTableView.cpp
    TTKMainWindow.cpp
    main.cpp
)

add_executable(TTKEditor
    ${TTKE_CPP}
    ${TTKE_Headers}
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
