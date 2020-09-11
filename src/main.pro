###QT project file
INCLUDEPATH += /usr/local/include/
LIBS += -L"/usr/local/lib/" -lcurl.4

QT += widgets charts

SOURCES += main.cpp client.cpp \ 
            clientMarketFuncs.cpp clientOrderFuncs.cpp \
            exceptions.cpp
HEADERS += client.hpp exceptions.hpp
CONFIG += debug
TARGET = tradebot
DESTDIR = ../bin/
QMAKE_CXXFLAGS += -std=c++17


