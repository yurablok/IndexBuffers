TEMPLATE = app
CONFIG -= qt app_bundle
CONFIG += console c++11

LIBS += -lpthread

SOURCES += \
    main.cpp

HEADERS += \
    schema_generated.hpp
