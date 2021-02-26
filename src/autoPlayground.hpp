#ifndef AUTOPLAYGROUND_H
#define AUTOPLAYGROUND_H

#include <QTextEdit>

#define TICK_CANDLE 100

#include <iostream>
#include <cstdio>

#pragma push_macro("slots")
#undef slots
#define PY_SSIZE_T_CLEAN
#include <Python.h>

class AutoPlayground {
    QTextEdit* outputStream;

public:
    AutoPlayground();
    ~AutoPlayground();
};
#pragma pop_macro("slots")
#endif /* AUTOPLAYGROUND_H */
