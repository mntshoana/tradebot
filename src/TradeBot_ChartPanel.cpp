//
//  TradeBot_ChartPanel.cpp
//  tradebot
//
//  Created by Motsoaledi Neo Tshoana on 2020/12/18.
//

#include "TradeBot_ChartPanel.hpp"

// Constructor
CandleStick::CandleStick(unsigned long long timestamp,
                         float open, float close,
                         float high, float low)
                : open(open), close(close),
                  high(high), low(low), timestamp(timestamp) {
}

void CandleStick::color(QPainter &painter){
    if (open < close ) { /* Green */
        painter.setPen(QColor(110, 182, 139));
        painter.setBrush(QColor(110, 182, 139));
    }
    else { /* Red */
        painter.setPen(QColor(218, 89, 96));
        painter.setBrush(QColor(218, 89, 96));
    }
}

//------------------------------------------------

// Constructor
Chart::Chart (QWidget *parent) : QWidget(parent){
    setGeometry(100, 50, 980, 400);
    setAutoFillBackground(true);
    min = 0;
    max = 2000;
    setFocusPolicy(Qt::ClickFocus);
}

void Chart::append (unsigned long long timestamp, int open, int close, int high, int low) {
    candles.push_back(CandleStick(timestamp, open, close, high, low));
    if (low < min)
        min = low;
    if (high > max)
        max = high * 1.1;
    
}

std::vector<unsigned long long> Chart::timeAxis(){
    std::vector<unsigned long long> timeAxis;
    for (CandleStick& candle : candles){
        timeAxis.push_back(candle.timestamp);
    }
    return timeAxis;
}

void Chart::paintEvent(QPaintEvent * event){
    int x = 0;
    int width = 980 * 0.01;
    QPainter painter(this);
    for (CandleStick &candle : candles){
        candle.color(painter);
        painter.drawLine(x + (width/2), scaleY(candle.high), x + (width/2), scaleY(candle.low));
        int y1 = scaleY(candle.open);
        int y2 = scaleY(candle.close);
        QRect rect(x, std::max(y1, y2) , width, abs(y1 - y2));
        painter.drawRect(rect);
        x += 1.8 * width;
        
    }
}

void Chart::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Plus:
        //zoomIn();
        break;
    case Qt::Key_Minus:
        //zoomOut();
        break;
    case Qt::Key_Left:
        scroll(-10, 0);
        break;
    case Qt::Key_Right:  
        scroll(10, 0);
        break;
    case Qt::Key_Up:
        scroll(0, 10);
        break;
    case Qt::Key_Down:
        //if (axisY->min() - 10 > 0)
            scroll(0, -10);
        break;
    default:
        //QGraphicsView::keyPressEvent(event);
        QWidget::keyPressEvent(event);
        break;
    }
    update();
}

int Chart::scaleY(int y){
    return 400 - int((y / (float)max ) * 400);
}

//-------------------------------------------------

//Constructor
ChartPanel::ChartPanel(QWidget* parent) : QWidget(parent) {
    setGeometry(0, 0, 1180, 500);
    
    area = new QScrollArea(this);
    chart = new Chart(area);
    area->setGeometry(100, 50, 980, 400);
    area->setWidget(chart);
    
    resetView = new QPushButton(this);
    resetView->setGeometry(0, 60, 90, 30);
    resetView->setText("Reset View");
    
    QStringList timeframeList;
    timeframeList << "1 MONTH"
                << "2 WEEKS" << "1 WEEK" << "1 DAY"
                << "4 HOURS" << "1 HOUR" << "30 MINUTES" << "5 MINUTES";
    timeframe = new QComboBox(this);
    timeframe->setGeometry(690, 28, 120, 18);
    timeframe->addItems(timeframeList);
    timeframe->setCurrentIndex(0);
    
    setAutoFillBackground(true);
    setFocusPolicy(Qt::ClickFocus);
}

void ChartPanel::paintEvent(QPaintEvent *e){
    if (chart->count() == 0)
        return;
    int x = 100;
    int width = 980 * 0.01;
    QPainter painter(this);
    std::vector<unsigned long long> timeAxis = chart->timeAxis();
    for (size_t i = 0; i < timeAxis.size(); i++) {
        if (i % 5 == 0 && x < 1080) {
            painter.drawLine(x +(width/2), 454, x+(width/2), 453);
            painter.setFont(QFont("Arial", 12));
            painter.drawText(QRect(x,459, 60, 30), Qt::AlignLeft,
                             QDateTime::fromMSecsSinceEpoch(timeAxis[i]).toString("dd MMM, yyyy").toStdString().c_str());
        }
        x += 1.8 * width;
    }
    
    float y = chart->getMax();
    float lowerBound = chart->getMin();
    float dif = y - lowerBound;
    for (size_t yAxis = 50; yAxis < 450; yAxis += 40) {
        
        painter.drawLine(1090, yAxis, 1092, yAxis);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(QRect(1095,yAxis-7, 40, 30), Qt::AlignLeft, std::to_string(y).c_str());
        y -= dif / 10;
    }
}

void ChartPanel::loadChart(std::vector<Luno::Trade>::iterator itr,
                           std::vector<Luno::Trade>::iterator end){
    int open, close, high, low;
    unsigned long long timestamp;
    std::string period = timeframe->currentText().toStdString();
    unsigned long long timeInterval;
    if (itr != end){
        open = close = high = low = itr->price;
        timestamp = itr->timestamp;
        timeInterval = updateInterval(period, timestamp);
        itr++;
    }
    while (itr != end) {
        if (high < itr->price)
            high = itr->price;
        if (low > itr->price)
            low = itr->price;
        close = itr->price;
            
        if (itr + 2 != end) {
            if ((itr+1)->timestamp > timeInterval) {
                chart->append(itr->timestamp, open, close, high, low);
                high = low = open = close = (itr+1)->price;
                //*catagories << QDateTime::fromMSecsSinceEpoch(set->timestamp()).toString("dd-MM-yyyy");
                timestamp = (itr+1)->timestamp;
                timeInterval = updateInterval(period, timestamp);
            }
        }
        if (itr + 1 == end){
            chart->append(timestamp, open, close, high, low);
            //*catagories << QDateTime::fromMSecsSinceEpoch(set->timestamp()).toString("dd-MM-yyyy");
        }
        itr++;
    }
         
        /*
        if (!chart->isVisible()){
            axisX = qobject_cast<QtCharts::QBarCategoryAxis*>(chart->axes(Qt::Horizontal).at(0));
            axisY = qobject_cast<QtCharts::QValueAxis*>(chart->axes(Qt::Vertical).at(0));
            axisX->setCategories(*catagories);
            if (axisY->max() < *limit){
                axisY->setRange(0, *limit);
            }
            axisY->setTickCount(10);
            axisY->setTickInterval(30000);
            axisY->setTickAnchor(0);
            axisY->applyNiceNumbers();
            axisY->setLabelFormat("  %7.0f");
            chart->setVisible(true);
            chart->removeAxis(axisY);
            chart->addAxis(axisY, Qt::AlignRight);
            series->attachAxis(axisY);
            axisY->setTickType(QValueAxis::TicksDynamic);
            resetView->setVisible(true);
            timeframe->setVisible(true);
        }
        *timestamp = ticks.back().timestamp
    }*/
}

unsigned long long ChartPanel::updateInterval(const std::string& period,
                                              unsigned long long timestamp){
    QDate date = QDateTime::fromMSecsSinceEpoch(timestamp).date();
    if (period == "1 MONTH"){
        QTime day(23,59,59,999);
        QDateTime monthEnd(QDate(date.year(), date.month(), date.daysInMonth()), day); // +8 UTC
        return monthEnd.toMSecsSinceEpoch();
    }
    else if (period == "2 WEEKS")
        ;
    else if (period == "1 WEEK")
        ;
    else if (period == "1 DAY"){
        QTime day(23,59,59,999);
        QDateTime dayEnd(QDate(date.year(), date.month(), date.day()), day); // +8 UTC
        return dayEnd.toMSecsSinceEpoch();
    }
    else if (period == "4 HOURS")
        ;
    else if (period == "1 HOUR")
        ;
    else if (period == "30 MINUTES")
        ;
    else if (period == "5 MINUTES")
        ;
    return 0;
}

void ChartPanel::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Plus:
        //zoomIn();
        break;
    case Qt::Key_Minus:
        //zoomOut();
        break;
    case Qt::Key_Left:
        area->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        area->scroll(10, 0);
        break;
    case Qt::Key_Up:
        area->scroll(0, 10);
        break;
    case Qt::Key_Down:
        //if (axisY->min() - 10 > 0)
            area->scroll(0, -10);
        break;
    default:
        //QGraphicsView::keyPressEvent(event);
        QWidget::keyPressEvent(event);
        break;
    }
    update();
}
