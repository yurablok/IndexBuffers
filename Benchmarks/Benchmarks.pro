QT -= gui core

CONFIG += console
CONFIG -= app_bundle

# run as: $ qmake DEFINES+=CUSTOM_COMPILER
contains(DEFINES, CUSTOM_COMPILER) {
    message("Using custom compiler");

    CONFIG += c++11
    QMAKE_CXXFLAGS += -std=c++1y
    # ------------------------------------------------------
    QMAKE_CC                = /usr/bin/gcc-4.9
    QMAKE_CXX               = /usr/bin/g++-4.9
    QMAKE_LINK              = /usr/bin/g++-4.9
    QMAKE_LINK_SHLIB        = /usr/bin/g++-4.9

    #QMAKE_AR               = /usr/bin/ar
    QMAKE_OBJCOPY           = /usr/bin/objcopy
    QMAKE_STRIP             = /usr/bin/strip
    # ------------------------------------------------------
    QMAKE_CXXFLAGS          += -w
    QMAKE_CFLAGS            += -w
} else {
    CONFIG += c++11
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    run_protobuf_v2.cpp \
    run_protobuf_v2lite.cpp \
    run_protobuf_v3.cpp \
    protobuf/addressbook_v2.pb.cc \
    protobuf/addressbook_v2lite.pb.cc \
    protobuf/addressbook_v3.pb.cc

HEADERS += \
    run_protobuf.h \
    flatbuffers/addressbook_generated.h \
    IndexBuffers/addressbook_generated.hpp \
    protobuf/addressbook_v2.pb.h \
    protobuf/addressbook_v2lite.pb.h \
    protobuf/addressbook_v3.pb.h

#INCLUDEPATH += $$PWD/protobuf
#DEPENDPATH += $$PWD/protobuf
unix:!macx: LIBS += -L$$PWD/protobuf/ -lprotobuf
unix:!macx: PRE_TARGETDEPS += $$PWD/protobuf/libprotobuf.a
