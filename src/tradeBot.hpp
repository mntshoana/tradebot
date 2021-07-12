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
   // void loadLocalTicks();
    //void downloadTicks(std::string pair);
    
    std::string lastTrades();
    std::string path;
private:
    std::thread thread;
    bool loadingTicks;
    bool closing;
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
    void Cleanup();
    
    signals:
    void finishedUpdate();

};
#endif /* TradeBot_hpp */
