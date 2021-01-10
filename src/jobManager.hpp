#ifndef JobManager_hpp
#define JobManager_hpp

#include "lunoclient.hpp"
#include "localbitcoinClient.hpp"

#include <queue>

#include <chrono>
#include <thread>
#include <QTimer>

class JobBase {
protected:
    void* outputStream;
    void* (*request)();
public:
    virtual void performJob() {} // nothing to perform
public:
    JobBase (void* stream, void* (*request)());
};

//

template <class stream, class res>
class Job : public JobBase {
public:
    virtual void performJob() override;
public:
    Job (stream* outputStream, res (*request)());
};

//

class JobManager : public QObject {
Q_OBJECT
public:
    JobManager(QObject* parent = nullptr);
        ~JobManager();
private:
    QTimer* timer;
    std::queue<JobBase> marketQueue, fasterQueue;
    
    void onUpdate();
public:
    void enqueue(JobBase job, bool isMarket);
    
};
#endif /* JobManager_hpp */
