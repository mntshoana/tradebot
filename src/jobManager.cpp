#include "jobManager.hpp"

JobBase::JobBase (void* stream, void* (*request)(), void* (*preprocessor)() ){
    outputStream = stream;
    this->request = request;
    this->preprocessor = preprocessor;
}


JobManager::JobManager(QObject *parent) : QObject(parent) {
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &JobManager::onUpdate);
    timer->start(1000);
}

JobManager::~JobManager(){
    delete timer;
}

void JobManager::onUpdate(){
    // every second
    if (!marketQueue.empty()) {
        marketQueue.front()->performJob();
        // deqeue 1 from marketQueue
        // complete task
        marketQueue.pop();
    }
    // dequeue 5 from fastQueue
    // repeat
    size_t count = 5;
    while (count > 0){
        if (fasterQueue.empty())
            break;
        fasterQueue.front()->performJob();
        delete fasterQueue.front();
        fasterQueue.pop();
        count--;
    }
}

void JobManager::enqueue(JobBase* job, bool isMarket){
    if (isMarket)
        marketQueue.push(job);
    else
        fasterQueue.push(job);
    // add to relavent queue
}


