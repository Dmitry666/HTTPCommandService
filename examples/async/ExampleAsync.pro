#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T23:33:20
#
#-------------------------------------------------

TARGET = AuthorizationModule
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
    authorizationmodule.h \
    usercontroller.h
SOURCES += \ 
    authorizationmodule.cpp \
    usercontroller.cpp

LIBS += -lHTTPCommandService
