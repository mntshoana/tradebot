#ifndef TradeBot_ChartPanel_hpp
#define TradeBot_ChartPanel_hpp

#include <QComboBox>
#include <QPushButton>
#include <QPainter>
#include <QColor>

#include <QtCore/QDateTime>
#include <QtWidgets/QGesture>
#include <QKeyEvent>
#include <QPaintEvent>

#include <QScrollArea>

#include <vector>

#include "lunoclient.hpp"

class CandleStick {
public:
    float open, close;
    float high, low;
    unsigned long long timestamp;
    
    CandleStick(unsigned long long timestamp,
                float open, float close,
                float high, float low);
    void color(QPainter &painter);
};

//------------------
class Chart : public QWidget {
    Q_OBJECT
    std::vector<CandleStick> candles;
    int min, max;
    int scaleY(int y);
public:
    Chart( QWidget *parent = nullptr);
    
    int getMin() const {return min;}
    int getMax() const {return max;}
    
    int left, top;
    int scaledIncrements;
    float wIncrement;
    void append(unsigned long long timestamp, int open, int close, int high, int low);
    std::vector<unsigned long long> timeAxis();
    size_t count() {return candles.size(); }
    void clear() {candles.clear();}
    virtual void paintEvent(QPaintEvent *e) override;
    //virtual void wheelEvent(QWheelEvent *event) override;
};

class ChartPanel : public QWidget {
public:
    Chart* chart;
    QPushButton* resetView;
    QComboBox* timeframe;
    
    ChartPanel(QWidget* parent = nullptr);
    
    void loadChart(std::vector<Luno::Trade>::iterator itr,
                     std::vector<Luno::Trade>::iterator end);
    unsigned long long updateInterval(const std::string& period, unsigned long long timestamp);
        
    virtual void paintEvent(QPaintEvent * e) final;
    virtual void keyPressEvent (QKeyEvent *event) override;
};

#endif /* TradeBot_ChartPanel_hpp */
