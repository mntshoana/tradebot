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
    
}

Job dequeue(){
    return Job();//still incomplete
}

void enqueue(Job job){
    
}
