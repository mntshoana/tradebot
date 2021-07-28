#ifndef AUTO_PLAYGROUND_HEADER
#define AUTO_PLAYGROUND_HEADER

#include <QTextEdit>

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

class AutoPlayground  : public QWidget {
    TextPanel text;
    FILE* script;
    std::string file, filepath;
    std::wstring wfilepath;
    
    wchar_t** argv;
public:
    AutoPlayground(QWidget* parent = nullptr);
    ~AutoPlayground();
    void runScript();
};
#endif /* AUTOPLAYGROUND_H */
