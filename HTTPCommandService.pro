#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T23:33:20
#
#-------------------------------------------------

TARGET = HTTPCommandService
TEMPLATE = app

DEFINES += WITH_JAVASCRIPT WITH_COOKIE
CONFIG += c++11

win32 {
    # Boost
    INCLUDEPATH += $(EXTERNALDIR)/boost_1_54_0
}

HEADERS += \
    connection.hpp \
    connection_manager.hpp \
    header.hpp \
    mime_types.hpp \
    reply.hpp \
    request.hpp \
    request_handler.hpp \
    request_parser.hpp \
    server.hpp \
    controller.h \
    common.h \
    defaultcontroller.h \
    delegate.h \
    helpcontroller.h \
    javascriptcontroller.h \
    modulemanager.h \
    module.h \
    directoryinfo.h \
    fileinfo.h \
    javascriptmanager.h \
    sessionmanager.h

SOURCES += \
    connection.cpp \
    connection_manager.cpp \
    main.cpp \
    mime_types.cpp \
    reply.cpp \
    request_handler.cpp \
    request_parser.cpp \
    server.cpp \
    controller.cpp \
    defaultcontroller.cpp \
    helpcontroller.cpp \
    javascriptcontroller.cpp \
    modulemanager.cpp \
    directoryinfo.cpp \
    fileinfo.cpp \
    javascriptmanager.cpp \
    sessionmanager.cpp


win32 {
    BOOST_VER = 1_54
    COMPILER_SHORT = mgw48

    debug {
        BOOST_SUFFIX = $${COMPILER_SHORT}-sd-$${BOOST_VER}
    }

    release {
        BOOST_SUFFIX = $${COMPILER_SHORT}-s-$${BOOST_VER}
    }

    LIBS += -L$(EXTERNALDIR)/boost_1_54_0/stage/lib

    LIBS += -lboost_system-$${BOOST_SUFFIX} -lboost_filesystem-$${BOOST_SUFFIX}

    debug {
    LIBS += -lboost_thread-mgw48-mt-sd-1_54
    }

    release {
    LIBS += -lboost_thread-mgw48-mt-s-1_54
    }

    LIBS += -lws2_32 -lmswsock
}

unix {
    LIBS += -L/opt/local/lib
    LIBS += -L/usr/local/lib
    LIBS += -Wl,-rpath=/usr/local/lib
    LIBS += -lboost_system -lboost_filesystem -lboost_thread -lboost_serialization
    LIBS += -lpthread

    #LIBS += -lssl -lcrypto
}
