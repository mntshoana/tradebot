#include "jobManager.hpp"
#include "window.hpp"
// LUNO allows 1 public request p/sec and
//             5 authenticated requests p/sec
// VALR allows 10 public requests p/min and (1 every 10 seconds)
//             3 authenticated requests p/sec


JobManager::JobManager() : QObject(nullptr) {
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
    int exchange = HomeView::getExchangeVal();
    /*Slow Queue*/
    // dequeue 1 per 10 seconds (VALR slow queue) #wait == false once every 10 seconds
    // dequeue 1 per second (Luno slow queue) #wait always == false
    bool wait = (exchange == VALR_EXCHANGE) && (timeElapsed % 10 != 0);
    
    while (!wait && !marketQueue.empty()) {
        if (abort)
            return;
        Task* job = marketQueue.front();
        marketQueue.pop();
        
        if (job->wait > 1){
            // skip job
            job->wait--;
            temporaryQueue.push(job);
            continue;
        }
        // complete task
        job->performJob();
    
        if (job->repeat){
            job->resetTimer();
            marketQueue.push(job);
        }
        else
            delete job;
        break;
    }
    // return skipped jobs to front of queue
    while (!marketQueue.empty()){
        temporaryQueue.push(marketQueue.front());
        marketQueue.pop();
    }
    temporaryQueue.swap(marketQueue);
    
    /*Fast Queue*/
    size_t count;
    if (exchange == LUNO_EXCHANGE) // dequeue 5 from fastQueue ( 5 per second)
        count = 5;
    if (exchange == VALR_EXCHANGE) // dequeue 3 from fastQueue (3 per second)
        count = 3;
    while (count > 0){
        if (fasterQueue.empty())
            break;
        if (abort)
            return;
        
        Task* job = fasterQueue.front();
        fasterQueue.pop();
        
        if (job->wait > 1){
            // skip job
            job->wait--;
            temporaryQueue.push(job);
            continue;
        }
        // complete task
        job->performJob();
        
        // note, only api calls are time constrained
        bool noEffectOnTime = job->isSetToAlwaysExecute();
        
        if (job->repeat){
            job->resetTimer();
            backOfTheQueue.push(job);
        }
        
        
        else
            delete job;
        
        if (noEffectOnTime)
            continue;
        else
            count--;
    }
    while (!fasterQueue.empty()){
        temporaryQueue.push(fasterQueue.front());
        fasterQueue.pop();
    }
    temporaryQueue.swap(fasterQueue);
    while (!backOfTheQueue.empty()){
        fasterQueue.push(backOfTheQueue.front());
        backOfTheQueue.pop();
    }
    
    busy = false;
    timeElapsed++; // update timeElapsed afterwards, not before!!
}

void JobManager::enqueue(Task* job){
    if (!job->isFast())
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
    while (!marketQueue.empty()) {
        marketQueue.pop();
    }
}

void JobManager::restart(){
    timer->stop();
    abort = false;
    timer->start(1000);
}
