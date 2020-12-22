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
Chart::Chart( QWidget *parent) : QWidget(parent){
    setGeometry(0, 0, 980, 400);
    setAutoFillBackground(true);
    min = 0;
    max = 2000;
    setFocusPolicy(Qt::ClickFocus);
    left = 0;
    top = 0;
    wIncrement = 980 * 0.01; // 1% of width
    scaledIncrements = 1.8 * wIncrement;
    setFocusPolicy(Qt::NoFocus);
    
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
    QPainter painter(this);
    for (size_t i = 0; i < candles.size(); i++) {
        if ( (i+1) * scaledIncrements < left && left > 0)
            continue;

        if ((i+1) * scaledIncrements > left + 980 + wIncrement)
            break;
        CandleStick& candle = candles.at(i);
        
        int x = i * scaledIncrements - left;
        candle.color(painter);
        painter.drawLine(x + (wIncrement/2), scaleY(candle.high) + top, x + (wIncrement/2), scaleY(candle.low) + top);
        int y1 = scaleY(candle.open);
        int y2 = scaleY(candle.close);
        QRect rect(x, std::max(y1, y2) + top , wIncrement, abs(y1 - y2));
        painter.drawRect(rect);
    }
}

int Chart::scaleY(int y){
    return 400 - int((y / (float)max ) * 400);
}

//-------------------------------------------------

//Constructor
ChartPanel::ChartPanel(QWidget* parent) : QWidget(parent) {
    setGeometry(0, 0, 1180, 500);
    chart = new Chart(this);
    chart->setGeometry(100, 50, 980, 400);
    
    resetView = new QPushButton(this);
    resetView->setGeometry(0, 60, 90, 30);
    resetView->setText("Reset View");
    connect(resetView, &QPushButton::clicked,
            this, [this](){chart->left = chart->top = 0; chart->update(); update();});
    
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
    
    QPainter painter(this);
    // X-Axis
    std::vector<unsigned long long> timeAxis = chart->timeAxis();
    std::string period = timeframe->currentText().toStdString();
    for (size_t i = 0; i < timeAxis.size(); i++) {
        if (i % 5 != 0)
            continue;
        
        if ( (i+5) * chart->scaledIncrements < chart->left && chart->left > 0)
            continue;
        if ( (i+5) * chart->scaledIncrements > chart->left + 980 + 100 + chart->wIncrement)
            break;
        
        int x = 100; // left margin
        x += i * chart->scaledIncrements - chart->left;
        std::string time = QDateTime::fromMSecsSinceEpoch(timeAxis[i]).toString("dd MMM, yyyy").toStdString();
        if (period == "4 HOURS" || period ==  "1 HOUR" ||
            period ==  "30 MINUTES" || period ==  "5 MINUTES")
            time = QDateTime::fromMSecsSinceEpoch(timeAxis[i]).toString("hh:mm dd.MM.yy").toStdString();
        
        painter.drawLine(x + (chart->wIncrement/2), 454, x+(chart->wIncrement/2), 455);
        painter.setFont(QFont("Arial", 12));
        if (x < 100)
            painter.drawText(QRect(100, 459, 70 - (100- x), 30), Qt::AlignRight,
                             time.c_str());
        else if (x+70 > 1080)
            painter.drawText(QRect(x,459, 1080 - x, 30), Qt::AlignLeft,
                         time.c_str());
        else
            painter.drawText(QRect(x,459, 70, 30), Qt::AlignLeft,
                         time.c_str());
    }
    
    // Y-Axis
    float y = chart->getMax();
    float lowerBound = chart->getMin();
    float dif = y - lowerBound;
    y += dif * chart->top / 400;
    for (size_t yAxis = 50; yAxis < 450; yAxis += 40) {
        
        painter.drawLine(1090, yAxis, 1092, yAxis);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(QRect(1095,yAxis-7, 40, 30), Qt::AlignLeft, std::to_string(y).c_str());
        y -= dif / 10;
    }
}

void ChartPanel::loadChart(std::vector<Luno::Trade>::iterator itr,
                           std::vector<Luno::Trade>::iterator end){
    if (chart->count() > 0)
        chart->clear();
    
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
}

unsigned long long ChartPanel::updateInterval(const std::string& period,
                                              unsigned long long timestamp){
    QDate date = QDateTime::fromMSecsSinceEpoch(timestamp).date();
    if (period == "1 MONTH"){
        QTime day(23,59,59,999);
        QDateTime monthEnd(QDate(date.year(), date.month(), date.daysInMonth()), day); // +8 UTC
        return monthEnd.toMSecsSinceEpoch();
    }
    else if (period == "2 WEEKS"){
        QTime day(23,59,59,999);
        QDateTime timeEnd(QDate(date), day); // +8 UTC
        return timeEnd.addDays(13).toMSecsSinceEpoch();
    }
    else if (period == "1 WEEK"){
        QTime day(23,59,59,999);
        QDateTime timeEnd(QDate(date), day); // +8 UTC
        return timeEnd.addDays(6).toMSecsSinceEpoch();
    }
    else if (period == "1 DAY"){
        QTime day(23,59,59,999);
        QDateTime dayEnd(QDate(date.year(), date.month(), date.day()), day); // +8 UTC
        return dayEnd.toMSecsSinceEpoch();
    }
    else if (period == "4 HOURS"){
        QDateTime day(QDateTime::fromMSecsSinceEpoch(timestamp));
        QTime time(day.time().addSecs(4 * 60 * 60));
        day.setTime(time);
        return day.toMSecsSinceEpoch();
    }
    else if (period == "1 HOUR"){
        QDateTime day(QDateTime::fromMSecsSinceEpoch(timestamp));
        QTime time(day.time().addSecs(60 * 60));
        day.setTime(time);
        return day.toMSecsSinceEpoch();
    }
    else if (period == "30 MINUTES"){
        QDateTime day(QDateTime::fromMSecsSinceEpoch(timestamp));
        QTime time(day.time().addSecs(30 * 60));
        day.setTime(time);
        return day.toMSecsSinceEpoch();
    }
        
    else if (period == "5 MINUTES"){
        {
            QDateTime day(QDateTime::fromMSecsSinceEpoch(timestamp));
            QTime time(day.time().addSecs(5 * 60));
            day.setTime(time);
            return day.toMSecsSinceEpoch();
        }
    }
    return updateInterval("1 MONTH", timestamp); // default
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
        chart->left -= 10;
        break;
    case Qt::Key_Right:
        chart->left += 10;
        break;
    case Qt::Key_Up:
        chart->top += 10;
        break;
    case Qt::Key_Down:
        if (chart->top <= 0)
            break;
        chart->top -= 10;
        break;
    default:
        //QGraphicsView::keyPressEvent(event);
        QWidget::keyPressEvent(event);
        break;
    }
    chart->update();
    update();
}
