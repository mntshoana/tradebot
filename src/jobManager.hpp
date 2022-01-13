#ifndef JobManager_hpp
#define JobManager_hpp

#include "job.hpp"
#include "client.hpp"

#include <queue>

#include <chrono>
#include <thread>
#include <QTimer>

#include <type_traits>

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
