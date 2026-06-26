###QT project file

QT += core widgets network webenginewidgets
QMAKE_CXXFLAGS += -std=c++17
CONFIG += c++17
CONFIG += object_parallel_to_source

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
            withdrawalBeneficiaries.cpp \
            textPanel.cpp \
            workspacePanel.cpp \
            window.cpp
HEADERS += tradeBot.hpp \
            orderPanel.hpp tradePanel.hpp \
            livePanel.hpp \
            pendingOrders.hpp \
            balancePanel.hpp withdrawPanel.hpp \
            withdrawalBeneficiaries.hpp \
            textPanel.hpp \
            workspacePanel.hpp \
            window.hpp
SOURCES += job.cpp jobManager.cpp
HEADERS += job.hpp jobManager.hpp
SOURCES += sidecar.cpp
HEADERS += sidecar.hpp
SOURCES += client/lunoclient/lunoTypes.cpp
HEADERS += client/lunoclient/lunoTypes.hpp
SOURCES += client/valrclient/valrTypes.cpp
HEADERS += client/valrclient/valrTypes.hpp
HEADERS += client/clientInterface.hpp
SOURCES += exceptions.cpp
HEADERS += exceptions.hpp
SOURCES += objectivec.mm
HEADERS += objectivec.h
HEADERS += autoPlayground.hpp
SOURCES += autoPlayground.cpp
SOURCES += main.cpp
OTHER_FILES += $$files(data/*.py, true)
OTHER_FILES += $$files(data/*.csv, true)

INCLUDEPATH += client/
