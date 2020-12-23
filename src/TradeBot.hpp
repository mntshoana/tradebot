#ifndef TradeBot_hpp
#define TradeBot_hpp

#include <chrono>
#include <thread>

#include <QApplication>
#include <QPalette>

#include <QTextEdit>
#include <QTextBrowser>

#include <QtCore/QDateTime>
#include <QTimer>

#include <QScrollBar>
#include <QLayout>

#include "lunoclient.hpp"
#include "localbitcoinClient.hpp"
#include "objectivec.h"

#include "TradeBot_OrderPanel.hpp"
#include "TradeBot_ChartPanel.hpp"

QTextEdit& operator<< (QTextEdit& stream, std::string str);
QTextBrowser& operator<< (QTextBrowser& stream, std::string str);

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
    void darkTheme();
    void lightTheme();
    
    std::string lastTrades();
private:
    bool nightmode;
    std::thread thread;
    
    QTimer* timer;
    QTextEdit* text;
    OrderPanel *orderPanel;
    ChartPanel *chartPanel;
    
    Luno::LunoClient lunoClient;
    LocalBitcoin::LocalBitcoinClient LocalBclient;

    size_t* count;
    
    std::fstream file;
    unsigned long long *timestamp, *latestTimestamp;
    float *high, *low, *open, *close;
    float *limit;
    
    std::vector<Luno::Trade> ticks, moreticks;
    
    private slots:
    void OnFinishedUpdate();
    void OnUpdate();
    
    signals:
    void finishedUpdate();

};
#endif /* TradeBot_hpp */
