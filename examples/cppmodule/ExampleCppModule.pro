#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T23:33:20
#
#-------------------------------------------------

TARGET = ExampleCppModule
TEMPLATE = lib

QMAKE_CXXFLAGS = -fPIC

DEFINES += \
    #WITH_JAVASCRIPT \
    WITH_COOKIE \
    WITH_JSONCPP \
    #WITH_TINYXML2 \
    #WITH_SSL \
    WITH_CTPP

CONFIG += c++11

INCLUDEPATH += ../../
LIBS += -L../../

win32 {
    mingw {
        INCLUDEPATH += /usr/include
        LIBS += -L/usr/lib
    }
    else {
        contains(DEFINES, WITH_JSONCPP) {
            JSONDIR = $(EXTERNALDIR)/jsoncpp-src-0.5.0
            INCLUDEPATH += $${JSONDIR}/include
            LIBS += -L$${JSONDIR}/lib
        }

        contains(DEFINES, WITH_TINYXML2) {
            TINYXMLDIR = $(EXTERNALDIR)/tinyxml2-master
            INCLUDEPATH += $${TINYXMLDIR}
            LIBS += -L$${TINYXMLDIR}/lib
        }

    }

    # Boost
    BOOSTDIR = $(EXTERNALDIR)/boost_1_54_0
    INCLUDEPATH += $${BOOSTDIR}
}

HEADERS += \
    examplecppmodule.h \
    examplecontroller.h
SOURCES += \ 
    examplecppmodule.cpp \
    examplecontroller.cpp


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

contains(DEFINES, WITH_JSONCPP) {
    LIBS += -ljsoncpp
}

contains(DEFINES, WITH_TINYXML2) {
    LIBS += -tinyxml2
}
