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

template <class T, class stream, class res, class param>
class Job1 : public JobBase{
    T* object;
    stream* outputStream;
    param arg;
    res (T::*request)(param);
public:
    virtual void performJob() override;
    
    inline Job1(T* object, stream* outputStream, res (T::*request)(param), param arg) {
        this->object = object;
        this->outputStream = outputStream;
        this->arg = arg;
        this->request = request;
    }
};

template <class T, class stream, class res, class param>
void Job1<T, stream, res, param>::performJob(){
    res result = (object->*request)(arg);
    (*outputStream) << result;
}

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
