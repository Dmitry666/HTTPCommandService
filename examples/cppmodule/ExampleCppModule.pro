#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T23:33:20
#
#-------------------------------------------------

TARGET = ExampleCppModule
TEMPLATE = lib

#QMAKE_CXXFLAGS = -fPIC

CONFIG += c++11

INCLUDEPATH += ../../
LIBS += -L../../

win32 {
    mingw {
        INCLUDEPATH += /usr/include
        LIBS += -L/usr/lib
    }
}

HEADERS += \
    examplecppmodule.h \
    examplecontroller.h
SOURCES += \ 
    examplecppmodule.cpp \
    examplecontroller.cpp

LIBS += -lHTTPCommandService
