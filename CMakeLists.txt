cmake_minimum_required(VERSION 3.16)
project(Tetris VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Network QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)
endif()

set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    menuwidget.cpp
    menuwidget.h
    tetriswidget.cpp
    tetriswidget.h
    tetromino.cpp
    tetromino.h
    apiclient.cpp
    apiclient.h
    mainwindow.ui
)

if(Qt6_FOUND)
    add_executable(Tetris ${SOURCES})
    target_link_libraries(Tetris PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
else()
    add_executable(Tetris ${SOURCES})
    target_link_libraries(Tetris PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
endif()

if(WIN32)
    set_target_properties(Tetris PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(APPLE)
    set_target_properties(Tetris PROPERTIES MACOSX_BUNDLE TRUE)
endif()
