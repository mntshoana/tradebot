#include "autoPlayground.hpp"
#include "window.hpp"

#define memAtPos(pointer, pos) (  (void*)(( (unsigned char*)pointer ) + pos) )

AutoPlaygroundPanel::AutoPlaygroundPanel(QWidget* parent) : QWidget(parent) {
    setObjectName("AutoPlaygroundPanel");
    
    TextPanel::init(parent);
    text = TextPanel::textPanel;
    
    view = new QGraphicsView(parent);
    scene = new QGraphicsScene;
    view->setScene(scene);
    
    viewLayout = new QVBoxLayout;
    viewLayout->addWidget(view);
    viewLayout->setContentsMargins(0, 0, 0, 0);
    viewLayout->setSpacing(0);
    setLayout(viewLayout);
    
    scriptfile = "autoPlayGroundScript.py";
    filepath = absolutePath();
    // NEED ABSOLUTE PATH TO "src/data/"
    // which is a few directories backwards
    size_t pos = filepath.find_last_of("/", filepath.length()-1);
    pos = filepath.find_last_of("/", pos-1);
    pos = filepath.find_last_of("/", pos-1);
    filepath = filepath.substr(0, pos) + "/src/data/";
    this->tickFile = HomeView::getTickFileName();
    
    imageData = nullptr;
    timer = new QTimer(parent);

    // WILL NOT ARBITRARILY RUN PYTHON SCRIPTS, BUT MIGHT SOON...
    *text << "Place script inside \"" + filepath + "\"\n";
    connect(timer, &QTimer::timeout, this, &AutoPlaygroundPanel::Onupdate);
    
    timer->start(4000);
    //
    runScript();
}

AutoPlaygroundPanel::~AutoPlaygroundPanel(){
    deleteSharedMem();
}

int AutoPlaygroundPanel::runScript() {
    int length = -1;
    
    if (POSIX_ENVIRONMENT){
        // Shared memory
        // Left half strictly for child write
        // right half strictly for parent write
        int shm_fd = shm_open("/autoPG", O_CREAT| O_RDWR, 0666);
        ftruncate(shm_fd, MEMSIZE);
        void* memptr = mmap(0, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (memptr == MAP_FAILED){
            *text << "Error! Unable to create shared memory";
            return -1;
        }
        imageData = (uchar*)memptr;
        ::close(shm_fd); // posix close, not qt close
        // clear memory
        memset(memptr, 0, MEMSIZE);
        
        // PROCESS CREATION
        pid_t pid = fork();
        if (pid < 0){ // error
            *text << "Error! Unable to call fork()";
            return -1;
        }
        else if (pid > 0){ // parent
            this->pid = pid;
            sleep(3);
            int responded;
            memcpy(&responded, memptr, sizeof(responded));
            *text << ("read response " + std::to_string(responded)).c_str();
            if (responded == 1){
                *text << "Autoplayground initialized."; // :)
                // submit ACK to child
                *((int*) memAtPos(memptr, 512)) = 1;
                sleep(3);
                
                // aquire image
                length = *((int*) memAtPos(memptr, IMGMEMSIZE));
                *text << "Successfully initialized autoplayground ";
            }
            if (responded == -1){
                deleteSharedMem();
                return -1;
            }
            if (responded == 0){
                *text << "Autoplayground initialized with no response from child."; // :)
            }
        }
        else if (pid == 0){ // child
            execlp("python3.8",
                  "python3.8",
                  (filepath + scriptfile).c_str(),
                  (filepath + tickFile).c_str(),
                  NULL);
            // only reaches here if it fails
            int failed = -1;
            memcpy(memptr, (void*)&failed, sizeof(failed));
            std::cout << "Error! Failed to run execl. Autoplayground failed to execute script";
            exit(-1);
        }
    }
    
    if (length > 0 & length < MEMSIZE){
        scene->addPixmap( QPixmap::fromImage(QImage::fromData(imageData, length)));
        view->fitInView(scene->sceneRect());
        view->show();
    }
    else if (length < 0 || length >= MEMSIZE){
        *text << ("Error! Data (length) read from shared memory is garbage, possible fault with script. Image length in bytes cannot be " + std::to_string(length) + ".").c_str();
    }


    return 0; // Success
}

void AutoPlaygroundPanel::deleteSharedMem(){
    if (POSIX_ENVIRONMENT){
        int result = -1;
        if (pid > 0)
            result = kill(pid,SIGHUP);
        if (result == 0)
            *text << "Exited child!";
        
        result = shm_unlink("/autoPG");
        if ( result == -1)
            *text << "Error! Unable to remove shared memory timeExercise";
    }
}

void AutoPlaygroundPanel::Onupdate(){
    scene->clear();
    int length = *((int*) memAtPos(imageData, IMGMEMSIZE));
    if (length > 0 & length < MEMSIZE){
    scene->addPixmap( QPixmap::fromImage(QImage::fromData(imageData, length)));
    view->fitInView(scene->sceneRect());
    }
    else if (length < 0 || length >= MEMSIZE){
        *text << ("Error! Data (length) read from shared memory is garbage, possible fault with script. Image length in bytes cannot be " + std::to_string(length) + ".").c_str();
    }
    view->update();
}

void AutoPlaygroundPanel::setGeometry(int ax, int ay, int aw, int ah) {
    QWidget::setGeometry(ax, ay, aw, ah);
    view->setGeometry(0, 0, aw, ah);
}

void AutoPlaygroundPanel::lightTheme(){
    QBrush brush(Qt::white, Qt::SolidPattern);
    view->setBackgroundBrush(brush);
    if (POSIX_ENVIRONMENT && pid > 0){
        /*rgb 4 bytes*/
        *((int*) memAtPos(imageData, (IMGMEMSIZE+IMGLENGTH))) = 0XFFFFFFFF;
    }
}
void AutoPlaygroundPanel::darkTheme(){
    QBrush brush(QColor(0x1e,0x1e,0x1e), Qt::SolidPattern);
    view->setBackgroundBrush(brush);
    
    if (POSIX_ENVIRONMENT && pid > 0){
        /*rgb 4 bytes*/
        *((int*) memAtPos(imageData, (IMGMEMSIZE+IMGLENGTH))) = 0XFF1E1E1E;
    }
}
