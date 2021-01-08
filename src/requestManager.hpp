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

class requestManager : public QObject {
Q_OBJECT
public:  requestManager(QObject* parent = nullptr);
        ~requestManager();
private:
    QTimer* timer;
    std::queue<Job> marketQueue, fasterQueue;
    
    Job dequeue();
    void onUpdate();
public:
    void enqueue(Job job);
    
};
#endif /* requestManager_hpp */
