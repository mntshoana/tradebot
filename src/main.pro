###QT project file
INCLUDEPATH += /usr/local/include/
LIBS += -L"/usr/local/lib/" -lcurl.4
LIBS += -framework SystemConfiguration

QT += widgets network ##charts

TARGET = tradebot
CONFIG += debug
DESTDIR = ../bin/
QMAKE_CXXFLAGS += -std=c++17

SOURCES += TradeBot.cpp \
            Label.cpp OrderPanel.cpp ChartPanel.cpp \
            Window.cpp
HEADERS += TradeBot.hpp \
            Label.hpp OrderPanel.hpp ChartPanel.hpp \
            Window.hpp
SOURCES += lunoclientOrderFuncs.cpp lunoclientMarketFuncs.cpp
HEADERS += lunoTypes.hpp lunoclient.hpp
SOURCES += localbitcoinClient.cpp
HEADERS += localbitcoinClient.hpp
SOURCES += ../ext/hash-library/sha256.cpp
HEADERS += ../ext/hash-library/hmac.h ../ext/hash-library/sha256.h
SOURCES += client.cpp
HEADERS += client.hpp
SOURCES += exceptions.cpp
HEADERS += exceptions.hpp
SOURCES += objectivec.mm
HEADERS += objectivec.h 
SOURCES += main.cpp
            


