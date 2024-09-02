TEMPLATE = app
TARGET = windowmanager
CONFIG += console c++11
CONFIG += qt
CONFIG += release
CONFIG += no_accessibility
CONFIG += no_pro
QT += core gui widgets

SOURCES += main.cpp \
           windowmanager.cpp \
           konami_code_handler.cpp

HEADERS += windowmanager.h \
           konami_code_handler.h

DESTDIR = src

INSTALLS += target
target.path = /usr/bin
target.files = $$DESTDIR/windowmanager

target.extra = mv $$DESTDIR/windowmanager/windowmanager /usr/bin/cwm
