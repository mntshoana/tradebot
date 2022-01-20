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


    #define RGBMEMSIZE (4)
    #define IMGLENGTH (4)
    #define IMGMEMSIZE (10240*4 - IMGLENGTH)
    #define MEMSIZE (IMGMEMSIZE + IMGLENGTH + RGBMEMSIZE)

#else
#define POSIX_ENVIRONMENT false
#endif

class AutoPlaygroundPanel  : public QWidget {
    Q_OBJECT
    TextPanel* text;
    uchar* imageData;
    QTimer* timer;
    
    QGraphicsScene *scene;
    QGraphicsView* view;
    QVBoxLayout* viewLayout;
    
private:
#if POSIX_ENVIRONMENT
    pid_t pid = -1;
#endif
    std::string scriptfile, tickFile, filepath;

    wchar_t** argv;
public:
    AutoPlaygroundPanel(QWidget* parent = nullptr);
    ~AutoPlaygroundPanel();
    int runScript();
    void deleteSharedMem();
    
    
    void setGeometry(int ax, int ay, int aw, int ah);

    void lightTheme();
    void darkTheme();
public slots:
    void Onupdate();
};
#endif /* AUTOPLAYGROUND_H */
