###QT project file

QT += widgets network
QMAKE_CXXFLAGS += -std=c++17

LIBS += -framework SystemConfiguration

TARGET = tradebot
CONFIG += debug_and_release_target
CONFIG(debug, debug|release) {
    DESTDIR = ../bin/debug/
    OBJECTS_DIR = ../bin/debug/.obj/
} else {
    DESTDIR = ../bin/release/
    OBJECTS_DIR = ../bin/release/.obj/
}


SOURCES += tradeBot.cpp \
            label.cpp orderPanel.cpp openOrderPanel.cpp chartPanel.cpp \
            window.cpp
HEADERS += tradeBot.hpp \
            label.hpp orderPanel.hpp openOrderPanel.hpp chartPanel.hpp \
            window.hpp
SOURCES += jobManager.cpp
HEADERS += jobManager.hpp
SOURCES += lunoclientOrderFuncs.cpp lunoclientMarketFuncs.cpp
HEADERS += lunoTypes.hpp lunoclient.hpp
SOURCES += localbitcoinClient.cpp
HEADERS += localbitcoinClient.hpp
SOURCES += client.cpp
HEADERS += client.hpp
SOURCES += exceptions.cpp
HEADERS += exceptions.hpp
SOURCES += objectivec.mm
HEADERS += objectivec.h
HEADERS += autoPlayground.hpp
SOURCES += autoPlayground.cpp
SOURCES += main.cpp

### External #####
#hash-library#
INCLUDEPATH += ../ext/hash-library/
SOURCES += ../ext/hash-library/sha256.cpp
HEADERS += ../ext/hash-library/hmac.h ../ext/hash-library/sha256.h

#libcurl#
INCLUDEPATH += /usr/local/include/
LIBS += -L"/usr/local/lib/" -lcurl.4

#python#
QMAKE_CXXFLAGS += $$system(python3-config --cflags)
LIBS += $$system(python3-config --ldflags --embed)
