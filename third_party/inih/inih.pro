#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T23:33:20
#
#-------------------------------------------------

TARGET = inih
TEMPLATE = lib

CONFIG += c++11 staticlib

HEADERS += \
    ini.h

SOURCES += \
    ini.c

# Install.
win32{
    INSTALL_PREFIX = /usr/include/$$TARGET
    INSTALL_HEADERS = $$HEADERS

    for(header, INSTALL_HEADERS) {
        path = $${INSTALL_PREFIX}/$${dirname(header)}
        eval(headers_$${path}.files += $$header)
        eval(headers_$${path}.path = $$path)
        eval(INSTALLS *= headers_$${path})
    }
    #target_headers.files  = $$HEADERS
    #target_headers.path = /usr/include/$$TARGET
    #INSTALLS += target_headers

    target.path = /usr/lib/
    INSTALLS += target
}


unix{
    INSTALL_PREFIX = /usr/include/$$TARGET
    INSTALL_HEADERS = $$HEADERS

    for(header, INSTALL_HEADERS) {
        path = $${INSTALL_PREFIX}/$${dirname(header)}
        eval(headers_$${path}.files += $$header)
        eval(headers_$${path}.path = $$path)
        eval(INSTALLS *= headers_$${path})
    }
    #target_headers.files  = $$HEADERS
    #target_headers.path = /usr/include/$$TARGET
    #INSTALLS += target_headers

    target.path = /usr/lib/
    INSTALLS += target
}
