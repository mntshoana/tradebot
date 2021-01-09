#include "requestManager.hpp"

requestManager::requestManager(QObject *parent) : QObject(parent) {
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &requestManager::onUpdate);
    timer->start(1000);
}

requestManager::~requestManager(){
    delete timer;
}

void requestManager::onUpdate(){
        // every second
    // deqeue 1 from marketQueue
    // dequeue 5 from fastQueue
    // repeat
}

Job dequeue(){
    return Job();//still incomplete
    
    // from whcih queue?
}

void enqueue(Job job){
    // check job type
    // add to relavent queue
}
