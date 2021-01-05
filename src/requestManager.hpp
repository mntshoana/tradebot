#ifndef requestManager_hpp
#define requestManager_hpp

#include <QTimer>
#include <queue>

class Job {
    void* outputStream;
    template <class R, class T>
    R request(T);
    template <class R> R request();
};

class requestManger {
private:
    QTimer* timer;
    std::queue<Job> marketQuew, fasterQueue;
    
    Job dequeue();
public:
    void enqueue(Job job);
    
};
#endif /* requestManager_hpp */
