TEMPLATE = app
TARGET = windowmanager
CONFIG += console c++11
CONFIG += qml
CONFIG += release
CONFIG += no_accessibility
CONFIG += no_pro
CONFIG += qt
QT += core gui widgets

# Répertoire source
SOURCES += src/main.cpp \
           src/windowmanager.cpp

HEADERS += src/windowmanager.h

DESTDIR = src

INSTALLS += target
target.path = /usr/bin
