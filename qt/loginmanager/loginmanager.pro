QT       += core gui
QT       += widgets

TARGET = loginmanager
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    loginwindow.cpp

HEADERS += \
    loginwindow.h

LIBS += -lcrypt
DESTDIR = bin
QMAKE_CXXFLAGS += -Wall
