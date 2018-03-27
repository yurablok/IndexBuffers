TARGET = ibc
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../libs/googletest-release-1.8.0/

SOURCES += stdafx.cpp \
           main.cpp \
           inbcompiler.cpp

HEADERS += stdafx.h \
           clara.hpp \
           inbcompiler.h

PRECOMPILED_HEADER = stable.h

DISTFILES += WhitePaper.h
