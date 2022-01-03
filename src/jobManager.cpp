#include "jobManager.hpp"

Task::Task( std::function<void (void)> lamdaFunction, bool fastTrack  ){
    updateWaitTime(1);
    this->fastTrack = fastTrack;
    this->lamdaFunction = lamdaFunction;
}

void Task::updateWaitTime(int time) {
    wait = recommendedWait = time;
}

void Task::performJob() {
    try{
        lamdaFunction();
    } catch (ResponseEx ex){
        TextPanel::textPanel << ex.String();
    }
}

void Task::setAsFast() {
    fastTrack = true;
}
void Task::setAsSlow() { 
    fastTrack = false;
}
bool Task::isFast() {
    return fastTrack;
}
void Task::resetTimer() {
    wait = recommendedWait;
}

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
        if (job->repeat){
            job->resetTimer();
            backOfTheQueue.push(job);
        }
        else
            delete job;
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
    delete timer;
    while (!marketQueue.empty()) {
        marketQueue.pop();
    }
}
