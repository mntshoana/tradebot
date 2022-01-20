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


SOURCES += label.cpp \
            lineBlock.cpp \
            dropDownBox.cpp
HEADERS += label.hpp \
            lineBlock.hpp \
            dropDownBox.hpp  
SOURCES += tradeBot.cpp \
            orderPanel.cpp tradePanel.cpp \
            livePanel.cpp \
            pendingOrders.cpp \
            balancePanel.cpp withdrawPanel.cpp \
            textPanel.cpp \
            workspacePanel.cpp \
            window.cpp
HEADERS += tradeBot.hpp \
            orderPanel.hpp tradePanel.hpp \
            livePanel.hpp \
            pendingOrders.hpp \
            balancePanel.hpp withdrawPanel.hpp \
            textPanel.hpp \
            workspacePanel.hpp \
            window.hpp
SOURCES += job.cpp jobManager.cpp
HEADERS += job.hpp jobManager.hpp
SOURCES += client/lunoclient/lunoTypes.cpp  \
            client/lunoclient/marketFuncs.cpp \
            client/lunoclient/orderFuncs.cpp \
            client/lunoclient/transferFuncs.cpp \
            client/lunoclient/srFuncs.cpp \
            client/lunoclient/quoteFuncs.cpp \
            client/lunoclient/accountFuncs.cpp \
            client/lunoclient/beneficiariesFuncs.cpp
HEADERS += client/lunoclient/lunoTypes.hpp client/lunoclient/lunoclient.hpp
SOURCES +=  client/valrclient/valrTypes.cpp \
            client/valrclient/marketFuncs.cpp \
            client/valrclient/accountFuncs.cpp \
            client/valrclient/walletFuncs.cpp \
            client/valrclient/orderFuncs.cpp \
            client/valrclient/payService.cpp
HEADERS += client/valrclient/valrTypes.hpp \
            client/valrclient/valrclient.hpp
SOURCES += client/client.cpp \
            client/sha512.cpp
HEADERS += client.hpp \
            client/clientInterface.hpp client/sha512.hpp client/hmac.hpp
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
