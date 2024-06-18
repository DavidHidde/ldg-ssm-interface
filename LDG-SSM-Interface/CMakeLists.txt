cmake_minimum_required(VERSION 3.5)

project(LDG-SSM-Interface VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS OpenGL OpenGLWidgets Widgets)

set(PROJECT_SOURCES
        main.cpp
        ldg_ssm_interface.cpp
        ldg_ssm_interface.h
        ldg_ssm_interface.ui
)

set(MACOSX_BUNDLE_ICON_FILE icons.icns)
set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/icons.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LDG-SSM-Interface
        WIN32 MACOSX_BUNDLE
        ${app_icon_macos}
        assets.qrc
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        widgets/pannable_scroll_area.h widgets/pannable_scroll_area.cpp
        util/tree_functions.h util/tree_functions.cpp
        widgets/render_view.h widgets/render_view.cpp
        drawing/model/tree_draw_properties.h drawing/model/tree_draw_properties.cpp
        drawing/overlay_painter.h drawing/overlay_painter.cpp
        util/grid_controller.h util/grid_controller.cpp
        drawing/renderer.h drawing/renderer.cpp
        drawing/image_renderer.h drawing/image_renderer.cpp
        util/atlas_container.h util/atlas_container.cpp
        input/input_configuration.h input/input_configuration.cpp
        input/visualization_configuration.h input/visualization_configuration.cpp
        input/json.h input/json.cpp
        input/data_buffer.h input/data_buffer.cpp
        drawing/volume_raycaster.h drawing/volume_raycaster.cpp
        drawing/model/types.h
        drawing/model/mesh.h drawing/model/mesh.cpp
        input/data.h
        drawing/model/volume_draw_properties.h drawing/model/volume_draw_properties.cpp
        drawing/model/window_draw_properties.h
        util/screen_controller.h util/screen_controller.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET LDG-SSM-Interface APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(LDG-SSM-Interface SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(LDG-SSM-Interface
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(LDG-SSM-Interface PRIVATE .)
target_link_libraries(LDG-SSM-Interface PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt::OpenGL
    Qt::OpenGLWidgets
)

# BZip2 library
find_package(BZip2 REQUIRED)
include_directories(${BZIP2_INCLUDE_DIRS})
target_link_libraries(LDG-SSM-Interface PRIVATE ${BZIP2_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LDG-SSM-Interface)
endif()
set_target_properties(LDG-SSM-Interface PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LDG-SSM-Interface
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LDG-SSM-Interface)
endif()
