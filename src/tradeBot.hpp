#ifndef TradeBot_hpp
#define TradeBot_hpp

#include <QApplication>
#include <QPalette>

#include <QtCore/QDateTime>

#include "objectivec.h"

#include "window.hpp"

#include "jobManager.hpp"

class TradeBot : public QWidget
{
    Q_OBJECT
public:
    TradeBot (QWidget *parent = 0);
private:
    void updateInterval(const std::string& period);
    
private:
    std::thread thread;
    
    QTimer* timer;
    
    HomeView* home;
    Window* current;
    
    JobManager manager;
    
    size_t* timerCount;
    
    void downloadTickData();
    void loadTickData();
    
    bool eventFilter(QObject *obj, QEvent *event)override;
    void enqueueJob(Task* job);
    
    public slots:
    void onEnqueueUserOrder(std::string orderID);
    
    private slots:
    void onFinishedUpdate();
    void onUpdate();
    void cleanup();
    
    signals:
    void finishedUpdate();

};
#endif /* TradeBot_hpp */
