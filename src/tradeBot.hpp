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
    void downloadTicks(std::string pair);
    void loadLocalTicks();
    
    std::string lastTrades();
    std::string path;
private:
    std::thread thread;
    bool closing;
    QTimer* timer;
    
    HomeView* home;
    Window* current;
    
    JobManager manager;
    
    size_t* timerCount;
    
    std::fstream file;
    unsigned long long *timestamp;
    
    bool eventFilter(QObject *obj, QEvent *event)override;
    
    private slots:
    void OnFinishedUpdate();
    void OnUpdate();
    void Cleanup();
    
    signals:
    void finishedUpdate();

};
#endif /* TradeBot_hpp */
