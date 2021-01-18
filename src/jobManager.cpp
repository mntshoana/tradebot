#include "jobManager.hpp"

JobBase::JobBase (void* stream, void* (*request)(), void* (*preprocessor)() ){
    outputStream = stream;
    this->request = request;
    this->preprocessor = preprocessor;
}


JobManager::JobManager(QObject *parent) : QObject(parent) {
    abort = false;
    busy = false;
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &JobManager::onUpdate);
    timer->start(1000);
}

JobManager::~JobManager(){
    delete timer;
}

void JobManager::onUpdate(){
    // every second
    if (abort)
        return;
    busy = true;
    if (!marketQueue.empty()) {
        JobBase* job = marketQueue.front();
        if (abort)
            return;
        job->performJob();
        // deqeue 1 from marketQueue
        // complete task
        marketQueue.pop();
        if (job->repeat);
            marketQueue.push(job);
    }
    // dequeue 5 from fastQueue
    // repeat
    size_t count = 5;
    while (count > 0){
        if (fasterQueue.empty())
            break;
        if (abort)
            return;
        fasterQueue.front()->performJob();
        delete fasterQueue.front();
        fasterQueue.pop();
        count--;
    }
    busy = false;
}

void JobManager::enqueue(JobBase* job, bool isMarket){
    if (isMarket)
        marketQueue.push(job);
    else
        fasterQueue.push(job);
    // add to relavent queue
}

void JobManager::stop(){
    timer->stop();
    abort = true;
    while (busy)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    delete timer;
    while (!marketQueue.empty()) {
        marketQueue.pop();
    }
}
