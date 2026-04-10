QT += core gui widgets network

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    menuwidget.cpp \
    tetriswidget.cpp \
    tetromino.cpp \
    apiclient.cpp

HEADERS += \
    mainwindow.h \
    menuwidget.h \
    tetriswidget.h \
    tetromino.h \
    apiclient.h

FORMS += \
    mainwindow.ui
