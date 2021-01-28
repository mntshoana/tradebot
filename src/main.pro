###QT project file
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += ../ext/hash-library/
INCLUDEPATH += ../ext/eigen/

LIBS += -L"/usr/local/lib/" -lcurl.4
LIBS += -framework SystemConfiguration

QT += widgets network ##charts

TARGET = tradebot
CONFIG += debug
DESTDIR = ../bin/
QMAKE_CXXFLAGS += -std=c++17

SOURCES += tradeBot.cpp \
            label.cpp orderPanel.cpp chartPanel.cpp \
            window.cpp
HEADERS += tradeBot.hpp \
            label.hpp orderPanel.hpp chartPanel.hpp \
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
SOURCES += main.cpp

###Automation
HEADERS += neuralNetwork.hpp
SOURCES += neuralNetwork.cpp

## External
SOURCES += ../ext/hash-library/sha256.cpp
HEADERS += ../ext/hash-library/hmac.h ../ext/hash-library/sha256.h
            


