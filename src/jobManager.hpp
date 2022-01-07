#ifndef JobManager_hpp
#define JobManager_hpp

#include "window.hpp"

#include <queue>

#include <chrono>
#include <thread>
#include <QTimer>

#include <type_traits>
class Task {
protected:
    std::function<void (void)> lamdaFunction;
    int recommendedWait;
    bool fastTrack;
public:
    bool repeat;
    int wait;
    
    Task( std::function<void (void)> lamdaFunction, bool fastTrack = false);
    void performJob();
    void updateWaitTime(int time);
    void setAsFast();
    void setAsSlow();
    void setRepeat(bool status);
    bool isFast();
    void resetTimer();
    
};


class JobManager : public QObject {
Q_OBJECT
public:
    JobManager(QObject* parent = nullptr, int exchange = LUNO_EXCHANGE);
    ~JobManager();
private:
    int exchange;
    int timeElapsed;
    bool abort, busy;
    QTimer* timer;
    std::queue<Task*> marketQueue, fasterQueue;
    std::queue<Task*> temporaryQueue, backOfTheQueue;
    
    void onUpdate();
public:
    void enqueue(Task* job);
    void stop();
    
};

#endif /* JobManager_hpp */
