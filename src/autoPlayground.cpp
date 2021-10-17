#include "autoPlayground.hpp"
#include "window.hpp"

#define memAtPos(pointer, pos) (  (void*)(( (unsigned char*)pointer ) + pos) )

AutoPlaygroundPanel::AutoPlaygroundPanel(QWidget* parent) : QWidget(parent) {
    file = "helloWorld.py";
    filepath = absolutePath();
    // NEED ABSOLUTE PATH TO "src/data/"
    // which is a few directories backwards
    size_t pos = filepath.find_last_of("/", filepath.length()-1);
    pos = filepath.find_last_of("/", pos-1);
    pos = filepath.find_last_of("/", pos-1);
    filepath = filepath.substr(0, pos) + "/src/data/";
    
    imageData = nullptr;
    parentPointer = parent;
    timer = new QTimer(parent);

    // WILL NOT ARBITRARILY RUN PYTHON SCRIPTS, BUT MIGHT SOON...
    text << "Place script inside \"" + filepath + "\"\n";
    connect(timer, &QTimer::timeout, this, &AutoPlaygroundPanel::Onupdate);
    
    timer->start(1500);
    //
    runScript();

}

AutoPlaygroundPanel::~AutoPlaygroundPanel(){
    deleteSharedMem();
}

int AutoPlaygroundPanel::runScript() {
    int length;
    
    if (POSIX_ENVIRONMENT){
        // Shared memory
        // Left half strictly for child write
        // right half strictly for parent write
        int shm_fd = shm_open("/autoPG", O_CREAT| O_RDWR, 0666);
        ftruncate(shm_fd, MEMSIZE);
        void* memptr = mmap(0, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (memptr == MAP_FAILED){
            text << "Error! Unable to create shared memory";
            return -1;
        }
        ::close(shm_fd); // posix close, not qt close
        // clear memory
        memset(memptr, 0, MEMSIZE);
        
        // PROCESS CREATION
        pid_t pid = fork();
        if (pid < 0){ // error
            text << "Error! Unable to call fork()";
            return -1;
        }
        else if (pid > 0){ // parent
            this->pid = pid;
            sleep(2);
            int responded;
            memcpy(&responded, memptr, sizeof(responded));
            text << ("read response " + std::to_string(responded)).c_str();
            if (responded == 1){
                text << "Autoplayground initialized."; // :)
                // submit ACK to child
                *((int*) memAtPos(memptr, 511)) = 1;
                sleep(3);
                
                // aquire image
                imageData = (uchar*)memptr;
                length = *((int*) memAtPos(memptr, 10240*3-5));
                text << "Successfully initialized autoplayground ";
                text << ("Image length: " + std::to_string(length)).c_str();
            }
            if (responded == -1){
                deleteSharedMem();
                return -1;
            }
            if (responded == 0){
                text << "Autoplayground initialized with no response from child."; // :)
            }
        }
        else if (pid == 0){ // child
            execl("/usr/bin/python3",
                  "python3",
                  (filepath + "helloWorld.py").c_str(),
                  (filepath + "XBTZAR.csv").c_str(),
                  NULL);
            // if it fails
            int failed = -1;
            memcpy(memptr, (void*)&failed, sizeof(failed));
            text << "Error! Failed to run execl. Autoplayground failed to execute script";
            exit(-1);
        }
    }

    setLayout(new QVBoxLayout);
    view = new QGraphicsView(parentPointer);
    view->setGeometry(411-5, 660-5, 929,202);
    layout()->addWidget(view);
    layout()->setContentsMargins(0, 0, 0, 0);
    scene = new QGraphicsScene;
    view->setScene(scene);
    
    scene->addPixmap( QPixmap::fromImage(QImage::fromData(imageData, length)));
    view->show();


    return 0; // Success
}

void AutoPlaygroundPanel::deleteSharedMem(){
    if (POSIX_ENVIRONMENT){
        int result = -1;
        if (pid > 0)
            result = kill(pid,SIGHUP);
        if (result == 0)
            text << "Exited child!";
        
        result = shm_unlink("/autoPG");
        if ( result == -1)
            text << "Error! Unable to remove shared memory timeExercise";
    }
}

void AutoPlaygroundPanel::Onupdate(){
    scene->clear();
    int length = *((int*) memAtPos(imageData, 10240*3-5));

    scene->addPixmap( QPixmap::fromImage(QImage::fromData(imageData, length)));
    view->update();
}
