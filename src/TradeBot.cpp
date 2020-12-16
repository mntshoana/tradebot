#include "TradeBot.hpp"

#define CSV_FILE_PATH "/Users/macgod/Dev/tradebot2/tradebot/src/data/"

/*class Label : public QTextBrowser {
public:
    Label(std::string title, QWidget* parent = nullptr ) : QTextBrowser(parent) {
        setReadOnly(true);
        std::stringstream ss;
        ss << std::fixed;
        ss << R"(
                <style>
                table {width: 100%;}
                tr { padding: 15px;}
                td, th {
                    padding: 2px 4px 1px 2px;
                    text-align: center;
                }
                tr th {
                    color: rgb(173, 176, 182);
                }
                </style>
                <table width=100%>
                    <tr>)";
        ss << title;
        ss << "</tr>";
        ss << "</table>\n";
        setHtml(ss.str().c_str());
    }
};
*/
QTextEdit& operator<< (QTextEdit& stream, std::string str)
{
    stream.append(str.c_str());
    return stream;
}
QTextBrowser& operator<< (QTextBrowser& stream, std::string str)
{
    stream.append(str.c_str());
    return stream;
}

TradeBot::TradeBot (QWidget *parent ) : QWidget(parent) {
    text = new QTextEdit(this);
    text->setGeometry(0, 500, 1180, 220);
    text->setText("");
    
    orderPanel = new OrderPanel(this);
    connect(orderPanel->orderview, &QTextBrowser::anchorClicked, this, &TradeBot::clickedLink);
    connect(orderPanel->tradeview, &QTextBrowser::anchorClicked, this, &TradeBot::clickedLink);
    

    set   = nullptr; // A single candle is a set, which is appended to a series
    series = nullptr;
    catagories = new QStringList(); // Labels of x-axis
    
    chart = new Chart();
    chart->setContentsMargins(60, -5, 0, -20);
    chart->setTitle("Luno Historical Data");
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setVisible(true);
    chart->prepCandleStickSeries(series, "XBTZAR");
    chart->createDefaultAxes(); // after adding series (=after prepCandleStickSeries)
    chart->setVisible(false);
    
    chartView = new ChartView(chart, this);
    chartView->setGeometry(0, 0, 1180, 500);
    
    connect(this, &TradeBot::finishedUpdate, this, &TradeBot::OnFinishedUpdate);
    
    resetView = new QPushButton(chartView);
    resetView->setGeometry(0, 60, 90, 30);
    resetView->setText("Reset View");
    resetView->setVisible(false);
    connect(resetView, &QPushButton::clicked, this,[this](){
        chart->zoomReset();
        axisY->setRange(0, *limit);
        axisX->setRange(catagories->first(), catagories->last());
    });
    
    QStringList timeframeList;
    timeframeList << "1 MONTH"
                << "2 WEEKS" << "1 WEEK" << "1 DAY"
                << "4 HOURS" << "1 HOUR" << "30 MINUTES" << "5 MINUTES";
    timeframe = new QComboBox(chartView);
    timeframe->setGeometry(690, 28, 120, 18);
    timeframe->addItems(timeframeList);
    timeframe->setCurrentIndex(0);
    timeframe->setVisible(false);
    
    connect(timeframe, &QComboBox::currentTextChanged, this, [this](const QString &str){*text << str.toStdString();});
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
    
    timestamp = new unsigned long long(0);
    latestTimestamp = new unsigned long long(0);
    timeInterval = new unsigned long long(0);
    
    // candle stick pieces
    low = new float (-1.f);    open = new float (-1.f);
    high = new float (-1.f);   close = new float (-1.f);
    limit = new float(-1.f);
    
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &TradeBot::OnUpdate);
    count = new size_t(0); // counts the timeouts triggered by timer
    timer->start(100);
}

TradeBot::~TradeBot() {
    delete count;
    count = nullptr;
    
    delete timestamp;
    timestamp = nullptr;
    
    delete high; delete low; delete open; delete close; delete limit;
    high = low = open = close = limit = nullptr;
}

void TradeBot::darkTheme(){
    // Theme
    chart->setBackgroundBrush(QBrush( QColor(30,30,30) ));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelColor(Qt::white);
    
    chartView->setBackgroundBrush(QBrush(QColor(30,30,30)));
    
    axisX = qobject_cast<QtCharts::QBarCategoryAxis*>(chart->axes(Qt::Horizontal).at(0));
    axisY = qobject_cast<QtCharts::QValueAxis*>(chart->axes(Qt::Vertical).at(0));
    QColor dark(50,50,50);
    axisX->setGridLineColor(dark);
    axisX->setLinePenColor(dark);
    axisY->setLinePenColor(dark);
    axisY->setGridLineColor(dark);
    QBrush light(QColor(230,230,230));
    axisX->setLabelsBrush(light);
    axisY->setLabelsBrush(light);
    
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: #1e1e1e;
                                        color: white;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    nightmode = true;
}
void TradeBot::lightTheme(){
    // Theme
    chart->setBackgroundBrush(QBrush( Qt::white ));
    chart->setTitleBrush(QBrush(Qt::black));
    chart->legend()->setLabelColor(Qt::black);
    
    chartView->setBackgroundBrush(QBrush(Qt::white));
    
    axisX = qobject_cast<QtCharts::QBarCategoryAxis*>(chart->axes(Qt::Horizontal).at(0));
    axisY = qobject_cast<QtCharts::QValueAxis*>(chart->axes(Qt::Vertical).at(0));
    QColor light(230,230,230);
    axisX->setGridLineColor(light);
    axisX->setLinePenColor(light);
    axisY->setLinePenColor(light);
    axisY->setGridLineColor(light);
    QBrush dark(QColor(20,20,20));
    axisX->setLabelsBrush(dark);
    axisY->setLabelsBrush(dark);
    
    orderPanel->livetradeview->setStyleSheet(R"(QGroupBox {
                                        background-color: white;
                                        color: black;
                                        border: none;
                                 } QGroupBox::title {
                                        background-color:transparent;
                                 })");
    nightmode = false;
}
void TradeBot::OnUpdate() {
    timer->stop();
    if (*count > 60) {
        *count = 1;
        //text->clear();
    }
    
    if (*count == 0) { // Initiate app
        try{
            *(orderPanel->orderview) << lunoClient.getOrderBook("XBTZAR").Format();
            auto step = orderPanel->orderview->verticalScrollBar()->singleStep();
            orderPanel->orderview->verticalScrollBar()->setValue(step * 97.8);
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
        
        orderPanel->tradeview->setHtml(lastTrades().c_str());
        
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(1100));
            *latestTimestamp = lunoClient.getTicker("XBTZAR").timestamp;
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
        /*
        try {
            *text << LocalBclient.getBuyAds("cn", "China");
        //  *text << lunoClient.postLimitOrder("XBTZAR", "ASK", 0.01222739 , 250019);
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }*/
         
        orderPanel->tradeview->setHtml(lastTrades().c_str());
        thread = std::thread([this]{
            loadLocalTicks();
            emit finishedUpdate();
        });
        thread.detach();
    }
    else if (*count % 5 == 0){
        try{
            auto y = orderPanel->orderview->verticalScrollBar()->value();
            std::string update = lunoClient.getOrderBook("XBTZAR").Format();
            orderPanel->orderview->setHtml("");
            *(orderPanel->orderview) << update;
            if (y != 0)
                orderPanel->orderview->verticalScrollBar()->setValue(y);
            else{
                auto step = orderPanel->orderview->verticalScrollBar()->singleStep();
                orderPanel->orderview->verticalScrollBar()->setValue(step * 90);
            }
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
        
        auto y = orderPanel->tradeview->verticalScrollBar()->value();
        orderPanel->tradeview->setHtml(lastTrades().c_str());
        orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
        formCandles();
        
        if (*count % 30 == 0){
            //Theme
            if (nightmode && !isDarkMode())
                lightTheme();
            if (!nightmode && isDarkMode())
                darkTheme();
            //Check errors
            if (*latestTimestamp == 0){
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
                    *latestTimestamp = lunoClient.getTicker("XBTZAR").timestamp;
                }
                catch (ResponseEx ex){
                    *text << ex.String();
                }
            }
        }
        emit finishedUpdate();
    }
    else if (*count % 2 == 1 ){

        auto y = orderPanel->tradeview->verticalScrollBar()->value();
        orderPanel->tradeview->setHtml(lastTrades().c_str());
        orderPanel->tradeview->verticalScrollBar()->setValue(y);
        
        if (*latestTimestamp == 0 || *latestTimestamp < *timestamp) {
            // latestTimestamp could be 0 (by internet error)
            // or
            // offline data is indeed up to date
            thread = std::thread([this]{downloadTicks(1);} );
            thread.detach();
        }
        else {
            // offline data is not up to date
            thread = std::thread([this]{downloadTicks(20);} );
            thread.detach();
        }
    }
    else {
        emit finishedUpdate();
    }
}

void TradeBot::OnFinishedUpdate(){
    //*text << "Completed " + std::to_string(*count);
    if (*count == 0)
        formCandles();
    *count = *count +1;
    timer->start(1000);
}
void TradeBot::clickedLink(const QUrl& url){
    orderPanel->txtPrice->setText( url.path().toStdString().c_str());
    return;
}

QtCharts::QCandlestickSet* TradeBot::makeCandlestick(const long long timestamp,
                                           const float open, const float high,
                                           const float low, const float close)
{
    QtCharts::QCandlestickSet *candlestickSet = new QtCharts::QCandlestickSet(open, high, low, close, timestamp);
    QPen pen;
    if (open <= close)
        pen.setColor(QColor(110, 182, 139)); // Green
    else
        pen.setColor(QColor(218, 89, 96)); // Red
    candlestickSet->setPen(pen);
    return candlestickSet;
}

void TradeBot::loadLocalTicks(){
    file.open(std::string(CSV_FILE_PATH) + "XBTZAR.csv" , std::ios::in);
    if (file.good()){
        file >> ticks;
        if (ticks.size() > 0){
            if (ticks.back().sequence != ticks.size())
            {
                while (ticks.size() > 0 && ticks.back().sequence != ticks.size())
                    ticks.pop_back();
                file.close();
                file.open(CSV_FILE_PATH, std::ios::out | std::ios::app);
                file << ticks;
                file.close();
                remove( (std::string(CSV_FILE_PATH) + "XBTZAR.csv").c_str() );
                rename( (std::string(CSV_FILE_PATH) + "XBTZAR2.csv").c_str(),
                        (std::string(CSV_FILE_PATH) + "XBTZAR.csv" ).c_str());
            }
        }
        if (ticks.size() > 0){
            *timestamp = ticks.back().timestamp;
        }
        else
            *timestamp = QDate(2013, 1, 1).startOfDay().toMSecsSinceEpoch();
    }
    else {
        *timestamp = QDate(2013, 1, 1).startOfDay().toMSecsSinceEpoch();
        file.clear();
    }
    file.close();
}

void TradeBot::downloadTicks(){
    moreticks = lunoClient.getTrades("XBTZAR", *timestamp-1); // order = newest to oldest
    while (ticks.size() > 0
            && moreticks.size() > 0
            && moreticks.back().sequence <= ticks.back().sequence)
        moreticks.pop_back();
    std::reverse(moreticks.begin(), moreticks.end()); // order = oldest to newest
    while (moreticks.size() > 0
            && moreticks.back().sequence != ticks.size()+moreticks.size())
            moreticks.pop_back();
    // Ensure sequenctial timestamp
    for (unsigned short index = 0; index < moreticks.size(); index++){
        if (moreticks[index].timestamp < *timestamp)
                moreticks.erase( moreticks.begin() + index, moreticks.end());
    }
    if (moreticks.size() > 0){
        ticks.insert(ticks.end(), moreticks.begin(), moreticks.end());
        *timestamp = moreticks.back().timestamp;
        file.open( std::string(CSV_FILE_PATH) + "XBTZAR.csv", std::ios::out | std::ios::app);
        file << moreticks;
        file.close();
    }
    moreticks.clear();
}

void TradeBot::downloadTicks(size_t reps){
    for (int i = 1; i < reps; i++){
        try{
            downloadTicks();
        }
        catch (ResponseEx ex){
            *text << ex.String();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    }
    try{
        downloadTicks();
    }
    catch (ResponseEx ex){
        *text << ex.String();
    }
    emit finishedUpdate();
}

void TradeBot::updateInterval(const std::string& period){
    QDate date = QDateTime::fromMSecsSinceEpoch(*timestamp).date();
    if (period == "1 MONTH"){
        QTime day(23,59,59,999);
        QDateTime monthEnd(QDate(date.year(), date.month(), date.daysInMonth()), day); // +8 UTC
        *timeInterval = monthEnd.toMSecsSinceEpoch();
    }
    else if (period == "2 WEEKS")
        ;
    else if (period == "1 WEEK")
        ;
    else if (period == "1 DAY"){
        QTime day(23,59,59,999);
        QDateTime dayEnd(QDate(date.year(), date.month(), date.day()), day); // +8 UTC
        *timeInterval = dayEnd.toMSecsSinceEpoch();
    }
    else if (period == "4 HOURS")
        ;
    else if (period == "1 HOUR")
        ;
    else if (period == "30 MINUTES")
        ;
    else if (period == "5 MINUTES")
        ;
}
void TradeBot::formCandles(){
    if (ticks.size() == 0)
        return;
    // Form candle sticks from ticks
    std::string period = timeframe->currentText().toStdString();
    for (size_t i = 0; i < ticks.size(); i++){
        if (i == 0){
            *open = *low = *high = *close = ticks[0].price;
            *timestamp = ticks[0].timestamp;
            updateInterval(period);
            series->clear();
            catagories->clear();
        }
        if (*high < ticks[i].price)
            *high = ticks[i].price;
        if (*low > ticks[i].price)
            *low = ticks[i].price;
        *close = ticks[i].price;
        
        if (i < ticks.size() -1) {
            if (ticks[i+1].timestamp > *timeInterval) {
                set = makeCandlestick(ticks[i].timestamp, *open, *high, *low, *close);
                *high = *low = *open = *close = ticks[i+1].price;
                series->append(set);
                *catagories << QDateTime::fromMSecsSinceEpoch(set->timestamp()).toString("dd-MM-yyyy");
                if (set->high() > *limit)
                    *limit = set->high() * 1.01;
                *timestamp = ticks[i+1].timestamp;
                updateInterval(period);
            }
        }
        if (i == ticks.size()-1){
            set = makeCandlestick(ticks[i].timestamp, *open, *high, *low, *close);
            series->append(set);
            *catagories << QDateTime::fromMSecsSinceEpoch(set->timestamp()).toString("dd-MM-yyyy");
            if (set->high() > *limit)
                *limit = set->high() * 1.01;
        }
    }
     
    
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
    *timestamp = ticks.back().timestamp;
}

std::string TradeBot::lastTrades() {
    std::stringstream ss;
    ss << std::fixed;
    ss << R"(
            <style>
            table {width: 100%;}
            tr { padding: 15px;}
            a {
                color: inherit;
                text-decoration: none;
            }
            td {
                padding: 2px 1px 1px 1px;
                text-align: center;
            }
            .Ask a {color: rgb(222, 81, 65);}
            .Bid a {color: rgb(94, 186, 137);}
            </style>
            <table width=100%>)";
    
    if (ticks.size() > 0){
        
        for (size_t i = ticks.size() - 1; i >= ticks.size()- 21; i--){
            ss << "\n<tr> <a href=\"" << ticks[i].price << "\">";
            ss << "\n<td>" << QDateTime::fromMSecsSinceEpoch(ticks[i].timestamp).toString("hh:mm").toStdString() << "</td>";
            ss << (ticks[i].isBuy ? "\n<td class=Ask>" : "\n<td class=Bid>" ) ;
            ss << std::setprecision(0);
            ss << "<a href=\"" << ticks[i].price << "\">";
            ss  << ticks[i].price;
            ss << "</a></td>";
            ss << "\n<td>" << std::setprecision(6)<< ticks[i].volume << "</td>";
            ss << "\n</a></tr>";
        }
        ss << "</table>\n";
    }
    return ss.str();
}
