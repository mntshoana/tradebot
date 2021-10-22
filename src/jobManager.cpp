#include "jobManager.hpp"

// LUNO allows 1 public request p/sec and
//             5 authenticated requests p/sec
// VALR allows 10 public requests p/min and (1 every 10 seconds)
//             3 authenticated requests p/sec


JobManager::JobManager(QObject *parent, int exchange) : QObject(parent) {
    this->exchange = exchange;
    timeElapsed = 0;
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
    if (timeElapsed >= 60)
        timeElapsed = 0;
    // every second
    if (abort)
        return;
    busy = true;
    bool wait = (exchange == VALR_EXCHANGE) && (timeElapsed % 10 != 0);
    if (!wait && !marketQueue.empty()) {
        JobBase* job = marketQueue.front();
        if (abort)
            return;
        job->performJob();
        // deqeue 1 from marketQueue
        // complete task
        marketQueue.pop();
        if (job->repeat)
            marketQueue.push(job);
    }

    size_t count;
    if (exchange == LUNO_EXCHANGE) // dequeue 5 from fastQueue
        count = 5;
    if (exchange == VALR_EXCHANGE) // dequeue 3 from fastQueue
        count = 3;
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
    timeElapsed++; // update timeElapsed afterwards, not before!!
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
