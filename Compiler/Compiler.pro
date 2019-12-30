TARGET = ibc
TEMPLATE = app
CONFIG -= qt app_bundle
CONFIG += console
QMAKE_CXXFLAGS += -std=c++17

SOURCES += \
    ast_meta.cpp \
    ibc.cpp \
    ibc_cpp.cpp \
    ibc_parser.cpp \
    main.cpp \
    MurmurHash3.cpp \
    stdafx.cpp \
    Utf8Ucs2Converter.cpp

HEADERS += \
    ast_meta.h \
    clara.hpp \
    colored_cout.h \
    ibc.h \
    MurmurHash3.h \
    stdafx.h \
    Utf8Ucs2Converter \
    version.h

PRECOMPILED_HEADER = stdafx.h
