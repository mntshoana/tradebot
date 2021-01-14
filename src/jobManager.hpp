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
    void* (*preprocessor)();
public:
    virtual void performJob() {} // nothing to perform
public:
    JobBase (void* stream = nullptr, void* (*request)() =  nullptr, void* (*preprocessor)() = nullptr);
};

//

template <class T, class stream, class res, class proc = std::string>
class Job : public JobBase {
private:
    T* object;
    stream* outputStream;
    res (T::*request)();
    proc (res::*preprocessor)();
public:
    virtual void performJob() override;
    
    inline Job (T* object, stream* outputStream, res (T::*request)(), proc (res::*preprocessor)() = nullptr) {
        this->object = object;
        this->outputStream = outputStream;
        this->request = request;
        this->preprocessor = preprocessor;
    }
};

template <class T, class stream, class res, class proc>
void Job<T, stream, res, proc>::performJob(){
    res result = (object->*request)();
    if (preprocessor)
        (*outputStream) << (result.*preprocessor)();
    else
        (*outputStream) << result;
}
//

template <class T, class stream, class res, class param, class proc = std::string>
class Job1 : public JobBase{
    T* object;
    stream* outputStream;
    param arg;
    res (T::*request)(param);
    proc (res::*preprocessor)();
public:
    virtual void performJob() override;
    
    inline Job1(T* object, stream* outputStream, res (T::*request)(param), param arg, proc (res::*preprocessor)() = nullptr) {
        this->object = object;
        this->outputStream = outputStream;
        this->arg = arg;
        this->request = request;
        this->preprocessor = preprocessor;
    }
};

template <class T, class stream, class res, class param, class proc>
void Job1<T, stream, res, param, proc>::performJob(){
    res result = (object->*request)(arg);
    if (preprocessor)
        (*outputStream) << (result.*preprocessor)();
    else
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
