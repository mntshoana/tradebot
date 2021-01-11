#ifndef JobManager_hpp
#define JobManager_hpp

#include "lunoclient.hpp"
#include "localbitcoinClient.hpp"

#include "window.hpp"

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
    JobBase (void* stream = nullptr, void* (*request)() =  nullptr);
};

//

template <class T, class stream, class res>
class Job : public JobBase {
private:
    T* object;
    stream* outputStream;
    res (T::*request)();
public:
    virtual void performJob() override;
public:
    inline Job (T* object, stream* outputStream, res (T::*request)()) {
        this->object = object;
        this->outputStream = outputStream;
        this->request = request;
    }
};

template <class T, class stream, class res>
void Job<T, stream, res>::performJob(){
    res result = (object->*request)();
    (*outputStream) << result;
}
//

class JobManager : public QObject {
Q_OBJECT
public:
    JobManager(QObject* parent = nullptr);
        ~JobManager();
private:
    QTimer* timer;
    std::queue<JobBase*> marketQueue, fasterQueue;
    
    void onUpdate();
public:
    void enqueue(JobBase* job, bool isMarket);
    
};

#endif /* JobManager_hpp */
