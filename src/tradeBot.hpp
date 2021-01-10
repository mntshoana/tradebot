#ifndef TradeBot_hpp
#define TradeBot_hpp

#include <QApplication>
#include <QPalette>

#include <QtCore/QDateTime>

#include "jobManager.hpp"

#include "objectivec.h"

#include "window.hpp"

class TradeBot : public QWidget
{
    Q_OBJECT
public:
    TradeBot (QWidget *parent = 0);
    virtual ~TradeBot();
private:
    void updateInterval(const std::string& period);
    void loadLocalTicks();
    void downloadTicks();
    void downloadTicks(size_t reps);
    
    std::string lastTrades();
private:
    std::thread thread;
    QTimer* timer;
    
    HomeView* home;
    P2PView* p2p;
    Window* current;
    
    Luno::LunoClient lunoClient;
    LocalBitcoin::LocalBitcoinClient LocalBclient;
    JobManager manager;
    
    size_t* timerCount;
    
    std::fstream file;
    unsigned long long *timestamp, *latestTimestamp;
    
    std::vector<Luno::Trade> ticks, moreticks;
    
    private slots:
    void OnFinishedUpdate();
    void OnUpdate();
    
    signals:
    void finishedUpdate();

};
#endif /* TradeBot_hpp */
