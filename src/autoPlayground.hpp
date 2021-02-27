#ifndef AUTOPLAYGROUND_H
#define AUTOPLAYGROUND_H

#include <QTextEdit>
#include "objectivec.h"

#define TICK_CANDLE 100

#include <iostream>
#include <cstdio>

#pragma push_macro("slots")
#undef slots
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#pragma pop_macro("slots")

class AutoPlayground {
    QTextEdit* outputStream;
    FILE* script;
    std::string file, filepath;
    std::wstring wfilepath;
    
    wchar_t** argv;
public:
    AutoPlayground(QTextEdit* output);
    ~AutoPlayground();
    void runScript();
};
#endif /* AUTOPLAYGROUND_H */
