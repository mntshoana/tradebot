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
    
    
    bool eventFilter(QObject *obj, QEvent *event)override;
    
    private slots:
    void OnFinishedUpdate();
    void OnUpdate();
    void Cleanup();
    
    signals:
    void finishedUpdate();

};
#endif /* TradeBot_hpp */
