set(TTKE_Headers
    data/InputFileHandler.h
    data/InputFileModel.h
    data/PlayerTypeInstance.h
    data/RKGFileHandler.h
    data/RKGHeaderModel.h
    ui/InputFileMenu.h
    ui/InputTableView.h
    ui/TTKMainWindow.h
    include/Definitions.h
    include/RKGDefinitions.h
    )

set(TTKE_CPP
    data/InputFileHandler.cpp
    data/InputFileModel.cpp
    data/PlayerTypeInstance.cpp
    data/RKGFileHandler.cpp
    data/RKGHeaderModel.cpp
    ui/InputFileMenu.cpp
    ui/InputTableView.cpp
    ui/TTKMainWindow.cpp
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
    "${CMAKE_CURRENT_SOURCE_DIR}"
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

        set(TARGET_DEPENDENCY_FILES
            $<TARGET_FILE:Qt5::Widgets>
            $<TARGET_FILE:Qt5::Core>
            $<TARGET_FILE:Qt5::Gui>
        )

        add_custom_command(
                TARGET TTKEditor POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:TTKEditor>
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:TTKEditor>
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:TTKEditor>
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TARGET_DEPENDENCY_FILES} $<TARGET_FILE_DIR:TTKEditor>
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> "$<TARGET_FILE_DIR:TTKEditor>/platforms/$<TARGET_FILE_NAME:Qt5::QWindowsIntegrationPlugin>"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsVistaStylePlugin> "$<TARGET_FILE_DIR:TTKEditor>/styles/$<TARGET_FILE_NAME:Qt5::QWindowsVistaStylePlugin>"
        )

endif(WIN32)

target_link_libraries(TTKEditor Qt5::Widgets)
target_link_libraries(TTKEditor Qt5::Core)
target_link_libraries(TTKEditor Qt5::Gui)

if (WIN32)
    install(FILES "${TARGET_DEPENDENCY_FILES}"
            DESTINATION bin)
    install(FILES $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin>
        DESTINATION bin/platforms)
    install(FILES $<TARGET_FILE:Qt5::QWindowsVistaStylePlugin>
        DESTINATION bin/styles)
endif(WIN32)
