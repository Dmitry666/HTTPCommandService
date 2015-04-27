#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T23:33:20
#
#-------------------------------------------------

TARGET = HTTPCommandServer
TEMPLATE = app

CONFIG += c++11

win32 {
    mingw {
        INCLUDEPATH += /usr/include
        LIBS += -L/usr/lib
    }
}

SOURCES += main.cpp

LIBS += -lHTTPCommandService
