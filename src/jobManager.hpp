#ifndef JobManager_hpp
#define JobManager_hpp

#include "window.hpp"

#include <queue>

#include <chrono>
#include <thread>
#include <QTimer>

#include <type_traits>

class JobBase {
protected:
    void* outputStream;
    void* (*request)();
    void* (*preprocessor)();
public:
    virtual void performJob() {} // nothing to perform
    bool repeat;
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
    try{
        res result = (object->*request)();
        if (preprocessor)
            (*outputStream) << (result.*preprocessor)();
        else
            (*outputStream) << result;
    } catch (ResponseEx ex){
        *outputStream << ex.String(); // To do:: should be an error stream here
    }
}
//

template <class T, class stream,
            class res, class param, class error = void, class proc = std::string>
class Job1 : public JobBase{
    T* object;
    stream* outputStream;
    error* errorStream;
    param arg;
    res (T::*request)(param);
    proc (res::*preprocessor)();
public:
    virtual void performJob() override;
    
    inline Job1(T* object, stream* outputStream, res (T::*request)(param), param arg, proc (res::*preprocessor )() = nullptr, error* errorStream = nullptr, bool repeat = true) {
        this->object = object;
        this->outputStream = outputStream;
        this->arg = arg;
        this->request = request;
        this->preprocessor = preprocessor;
        this->repeat = repeat;
        this->errorStream = errorStream;
    }
};

template <class T, class stream, class res, class param, class error, class proc>
void Job1<T, stream, res, param, error,proc>::performJob(){
    if constexpr (!std::is_same_v<stream, unsigned long long>)
        try{
            res result = (object->*request)(arg);
            if (preprocessor){
                proc processedResults = (result.*preprocessor)();
                (*outputStream) << processedResults;
            }
            else {
                    (*outputStream) << result;
            }
        } catch (ResponseEx ex){
                (*outputStream) << ex.String(); // To do:: should be an error stream here
        }
    
    if constexpr (std::is_same_v<stream, unsigned long long>)
        try{
            if (preprocessor){
                res result = (object->*request)(arg);
                proc processedResults = (result.*preprocessor)();
                (*outputStream) = processedResults;
            }
        } catch (ResponseEx ex){
            if (errorStream)
                (*errorStream) << ex.String(); // To do:: should be an error stream here
        }
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
