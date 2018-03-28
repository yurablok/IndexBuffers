TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += main.cpp

HEADERS += schema_generated.h

INCLUDEPATH += ../../libs/googletest-release-1.8.0/
