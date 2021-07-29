#ifndef AUTO_PLAYGROUND_HEADER
#define AUTO_PLAYGROUND_HEADER

#include <QProcess>
#include "textPanel.hpp"
#include "objectivec.h"

#define TICK_CANDLE 100

#include <iostream>
#include <cstdio>

#pragma push_macro("slots")
#undef slots
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#pragma pop_macro("slots")

class AutoPlaygroundPanel  : public QWidget {
    Q_OBJECT
    TextPanel text;
    std::thread* thread;
    
    FILE* script;
    std::string file, filepath;
    std::wstring wfilepath;
    
    //QProcess *myProcess;
    wchar_t** argv;
public:
    AutoPlaygroundPanel(QWidget* parent = nullptr);
    ~AutoPlaygroundPanel();
    void run();
    void runScript();
};
#endif /* AUTOPLAYGROUND_H */
