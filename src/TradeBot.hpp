#ifndef TradeBot_hpp
#define TradeBot_hpp

#include <chrono>
#include <thread>

#include <QApplication>

#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTextBrowser>

#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>
#include <QtCharts/QChartView>

#include <QCategoryAxis>


#include <QtCore/QDateTime>
#include <QTimer>

#include <QScrollBar>
#include <QLayout>

#include "lunoclient.hpp"
#include "localbitcoinClient.hpp"
#include "objectivec.h"
#include "Chart.hpp"


QTextEdit& operator<< (QTextEdit& stream, std::string str);
QTextBrowser& operator<< (QTextBrowser& stream, std::string str);

class TradeBot : public QWidget
{
    Q_OBJECT
public:
    TradeBot (QWidget *parent = 0);
    virtual ~TradeBot();
private:
    QtCharts::QCandlestickSet* makeCandlestick(const long long timestamp,
                                               const float open, const float high,
                                               const float low, const float close);
    void updateInterval(const std::string& period);
    void loadLocalTicks();
    void downloadTicks();
    void downloadTicks(size_t reps);
    void formCandles();
    void darkTheme();
    void lightTheme();
private:
    bool nightmode;
    std::thread thread;
    
    QComboBox* timeframe;
    QPushButton* resetView;
    QTimer* timer;
    QTextEdit* text;
    QTextBrowser* orderview;
    
    Luno::LunoClient lunoClient;
    LocalBitcoin::LocalBitcoinClient LocalBclient;

    size_t* count;
    
    std::fstream file;
    unsigned long long* timestamp,* latestTimestamp;
    unsigned long long* timeInterval;
    float* high, *low, *open, *close;
    float* limit;
    
    std::vector<Luno::Trade> ticks, moreticks;
    
    QtCharts::QCandlestickSeries* series;
    QtCharts::QCandlestickSet *set;
    QStringList* catagories;
    
    Chart* chart;
    
    QtCharts::QBarCategoryAxis *axisX;
    QtCharts::QValueAxis *axisY;
    ChartView* chartView;
    private slots:
    void clickedLink(const QUrl& url);
    
    void OnUpdate();
    void OnFinishedUpdate();
    signals:
    void finishedUpdate();

};
#endif /* TradeBot_hpp */
