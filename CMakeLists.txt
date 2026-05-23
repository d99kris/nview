cmake_minimum_required(VERSION 3.16)
project(nview LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

add_executable(nview
    src/application.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/preloadthread.cpp
    src/application.h
    src/mainwindow.h
    src/preloadthread.h
    src/version.h
    src/mainwindow.ui
)

set_target_properties(nview PROPERTIES
    MACOSX_BUNDLE OFF
    WIN32_EXECUTABLE OFF
)

target_link_libraries(nview PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

include(GNUInstallDirs)
install(TARGETS nview RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES res/nview.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
