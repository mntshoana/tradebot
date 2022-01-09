###QT project file

QT += widgets network webenginewidgets
QMAKE_CXXFLAGS += -std=c++17
CONFIG += c++17

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
            label.cpp lineBlock.cpp \
            orderPanel.cpp tradePanel.cpp \
            livePanel.cpp \
            pendingOrders.cpp \
            balancePanel.cpp withdrawPanel.cpp \
            textPanel.cpp \
            workspacePanel.cpp \
            window.cpp
HEADERS += tradeBot.hpp \
            label.hpp lineBlock.hpp \
            orderPanel.hpp tradePanel.hpp \
            livePanel.hpp \
            pendingOrders.hpp \
            balancePanel.hpp withdrawPanel.hpp \
            textPanel.hpp \
            workspacePanel.hpp \
            window.hpp
SOURCES += job.cpp jobManager.cpp
HEADERS += job.hpp jobManager.hpp
SOURCES += lunoclient/lunoTypes.cpp  \
            lunoclient/marketFuncs.cpp lunoclient/orderFuncs.cpp \
            lunoclient/transferFuncs.cpp lunoclient/srFuncs.cpp \
            lunoclient/quoteFuncs.cpp lunoclient/accountFuncs.cpp \
            lunoclient/beneficiariesFuncs.cpp
HEADERS += lunoclient/lunoTypes.hpp lunoclient/lunoclient.hpp
SOURCES +=  valrclient/valrTypes.cpp \
            valrclient/marketFuncs.cpp \
            valrclient/accountFuncs.cpp \
            valrclient/walletFuncs.cpp \
            valrclient/orderFuncs.cpp \
            valrclient/payService.cpp
HEADERS += valrclient/valrTypes.hpp valrclient/valrclient.hpp
SOURCES += client.cpp \
            sha512.cpp
HEADERS += client.hpp \
            sha512.hpp hmac.hpp
SOURCES += exceptions.cpp
HEADERS += exceptions.hpp
SOURCES += objectivec.mm
HEADERS += objectivec.h
HEADERS += autoPlayground.hpp
SOURCES += autoPlayground.cpp
SOURCES += main.cpp
SOURCES += $$files(data/*.py, true)
SOURCES += $$files(data/*.csv, true)

### External #####
#libcurl#
INCLUDEPATH += /usr/local/include/
LIBS += -L"/usr/local/lib/" -lcurl.4
