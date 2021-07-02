TARGET = HTTPCommandService
TEMPLATE = lib

DEFINES += \
    #WITH_JAVASCRIPT \
    WITH_COOKIE \
    WITH_JSONCPP \
    #WITH_TINYXML2 \
    #WITH_SSL \
    WITH_CTPP \
    WITH_INICONFIG

DEFINES += CRT_SECURE_NO_WARNINGS

CONFIG += c++11
CONFIG += unversioned_soname unversioned_libname

CONFIG += conan_basic_setup
include(conan/conanbuildinfo.pri)

HEADERS += \
    common.h \
    common-private.h \
    controller.h \
    crypt.h \
    defaultcontroller.h \
    delegate.h \
    directoryinfo.h \
    fileinfo.h \
    helpcontroller.h \
    httpservice.h \
    javascriptcontroller.h \
    javascriptmanager.h \
    js-http-request-processor.h \
    module.h \
    modulemanager.h \
    server.h \
    service-config.h \
    sessionmanager.h \
    websocket/web_connection.hpp \
    websocket/web_connection_manager.hpp \
    websocket/web_reply.hpp \
    websocket/web_request.hpp \
    websocket/web_request_handler.hpp \
    websocket/web_request_parser.hpp \
    websocket/web_server.hpp \
    tcpnative/tcp_connection.hpp \
    tcpnative/tcp_connection_manager.hpp \
    tcpnative/tcp_reply.hpp \
    tcpnative/tcp_request.hpp \
    tcpnative/tcp_request_handler.hpp \
    tcpnative/tcp_request_parser.hpp \
    tcpnative/tcp_server.hpp \
    http/connection.hpp \
    http/connection_manager.hpp \
    http/header.hpp \
    http/mime_types.hpp \
    http/reply.hpp \
    http/request.hpp \
    http/request_handler.hpp \
    http/request_parser.hpp \
    http/server.hpp \
    http/sslconnection.hpp \
    http/sslserver.hpp \
    serverstats.h

SOURCES += \
    common-private.cpp \
    controller.cpp \
    defaultcontroller.cpp \
    directoryinfo.cpp \
    fileinfo.cpp \
    helpcontroller.cpp \
    httpservice.cpp \
    javascriptcontroller.cpp \
    javascriptmanager.cpp \
    js-http-request-processor.cpp \
    main.cpp \
    modulemanager.cpp \
    service-config.cpp \
    sessionmanager.cpp \
    websocket/web_connection.cpp \
    websocket/web_connection_manager.cpp \
    websocket/web_reply.cpp \
    websocket/web_request_handler.cpp \
    websocket/web_request_parser.cpp \
    websocket/web_server.cpp \
    tcpnative/tcp_connection.cpp \
    tcpnative/tcp_connection_manager.cpp \
    tcpnative/tcp_reply.cpp \
    tcpnative/tcp_request_handler.cpp \
    tcpnative/tcp_request_parser.cpp \
    tcpnative/tcp_server.cpp \
    http/connection.cpp \
    http/connection_manager.cpp \
    http/mime_types.cpp \
    http/reply.cpp \
    http/request_handler.cpp \
    http/request_parser.cpp \
    http/server.cpp \
    http/sslconnection.cpp \
    http/sslserver.cpp \
    serverstats.cpp
