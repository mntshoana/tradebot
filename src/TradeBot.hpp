#ifndef TradeBot_hpp
#define TradeBot_hpp

#include <chrono>
#include <thread>

#include <QApplication>
#include <QPalette>

#include <QtCore/QDateTime>
#include <QTimer>

#include "lunoclient.hpp"
#include "localbitcoinClient.hpp"
#include "objectivec.h"

#include "Window.hpp"

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
    
    HomeWindow* homeWindow;
    Luno::LunoClient lunoClient;
    LocalBitcoin::LocalBitcoinClient LocalBclient;

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
