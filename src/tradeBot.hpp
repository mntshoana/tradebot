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
    virtual ~TradeBot();
private:
    void updateInterval(const std::string& period);
    void loadLocalTicks();
    void downloadTicks(std::string pair);
    void downloadTicks(std::string pair, size_t reps);
    
    std::string lastTrades();
private:
    std::thread thread;
    bool loadingTicks;
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
    
    
    private slots:
    void OnFinishedUpdate();
    void OnUpdate();
    
    signals:
    void finishedUpdate();

};
#endif /* TradeBot_hpp */
