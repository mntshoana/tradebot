#include "jobManager.hpp"

JobBase::JobBase (void* stream, void* (*request)()){
    outputStream = stream;
    this->request = request;
}

template <class stream, class res>
Job<stream, res>::Job(stream* outputStream, res (*request)()){
    this->outputStream = outputStream;
    this->request = request;
}

template <class stream, class res>
void Job<stream, res>::performJob(){
    outputStream << (res) *request();
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
    JobBase task = marketQueue.front();
    // deqeue 1 from marketQueue
    // complete task
    task.performJob();
    marketQueue.pop();
    // dequeue 5 from fastQueue
    // repeat
    size_t count = 5;
    while (count > 0){
        if (fasterQueue.empty())
            break;
        task = fasterQueue.front();
        task.performJob();
        fasterQueue.pop();
        count--;
    }
}

void JobManager::enqueue(JobBase job, bool isMarket){
    if (isMarket)
        marketQueue.push(job);
    else
        fasterQueue.push(job);
    // add to relavent queue
}


