#ifndef AUTO_PLAYGROUND_HEADER
#define AUTO_PLAYGROUND_HEADER

#include <QImage>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsView>
#include <QTimer>
#include <QVBoxLayout>

#include "textPanel.hpp"
#include "objectivec.h"

#define TICK_CANDLE 100

#include <iostream>
#include <cstdio>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
// POSIX PROCESS CREATION AND SHARED MEMORRY WILL BE REQUIRED
#define POSIX_ENVIRONMENT true
    #include <sys/wait.h>
    #include <unistd.h> //  fork
    #include <sys/shm.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <string.h>
    #include <signal.h> // kill


    #define MEMSIZE (10240*3)
#else
#define POSIX_ENVIRONMENT false
#endif

class AutoPlaygroundPanel  : public QWidget {
    Q_OBJECT
    TextPanel text;
    uchar* imageData;
    QTimer* timer;
    
    QWidget* parentPointer;
    QGraphicsScene *scene;
    QGraphicsView* view;
    
private:
#if POSIX_ENVIRONMENT
    pid_t pid = -1;
#endif
    std::string file, filepath;
    std::wstring wfilepath;

    wchar_t** argv;
public:
    AutoPlaygroundPanel(QWidget* parent = nullptr);
    ~AutoPlaygroundPanel();
    int runScript();
    void deleteSharedMem();
    
public slots:
    void Onupdate();
};
#endif /* AUTOPLAYGROUND_H */
