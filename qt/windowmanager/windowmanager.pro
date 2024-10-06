TEMPLATE = app
TARGET = windowmanager
CONFIG += console c++11
CONFIG += qt
CONFIG += release
CONFIG += no_accessibility
CONFIG += no_pro
QT += core gui widgets

LIBS += -lX11
SOURCES += main.cpp \
           windowmanager.cpp \
           konami_code_handler.cpp \
           userinteractright.cpp \
           taskbar.cpp \
           win/topbar.cpp 
           
HEADERS += windowmanager.h \
           konami_code_handler.h \
           userinteractright.h \
           taskbar.h \
           win/topbar.h

DESTDIR = src

INSTALLS += target
target.path = /usr/bin
target.files = $$DESTDIR/windowmanager

target.extra = mv $$DESTDIR/windowmanager /usr/bin/a2wm 2> /dev/null
