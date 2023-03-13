QT += core network

CONFIG += c++11 console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tcp

SOURCES += main.cpp \
           server.cpp

HEADERS += server.h \
    constants.h

RESOURCES +=
