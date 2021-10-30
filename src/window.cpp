#include "window.hpp"
#include <QTimer>



HomeView::HomeView (QWidget *parent, int exchange) : QWidget(parent) {
    this->exchange = exchange;
    
    TextPanel::init(parent);
    
    livePanel = new LivePanel(parent);

    // Chart Displayed using QWebEngine browser
    view = new QWebEngineView(parent); // to do: move VIEW to a chart widget
    view->setGeometry(0, 0, 930, 500);
    std::string chartUrl = "";
    if (exchange == LUNO_EXCHANGE)
        chartUrl = "https://d32exi8v9av3ux.cloudfront.net/static/scripts/tradingview.prod.html?symbol=XBTZAR&res=60&lang=en";
    if (exchange == VALR_EXCHANGE)
        chartUrl = "https://www.valr.com/exchange/BTC/ZAR";
        
    view->load(QUrl(chartUrl.c_str()));
    
    workPanel = new WorkspacePanel(parent);
    workPanel->setGeometry(0, 472, 930, 248);
    
    
    // Note: the next line only works if the browser has already loaded, hence delay
    if (exchange == LUNO_EXCHANGE){
        if (!isDarkMode())
            QTimer::singleShot(4500, this, [this](){
                    view->page()->runJavaScript( R"java(
                    tvWidget.changeTheme("Light");
                    bgcolor(white);
                )java");
            });
    } else if (exchange == VALR_EXCHANGE){
        QTimer::singleShot(4500, this, [this](){
                view->page()->runJavaScript( R"java(
                document.getElementById("root").replaceWith(document.getElementById("tvChartWidget"));
            )java");
        });
    }
        
    
    view->show();
    
    // Theme
    if (isDarkMode())
        darkTheme();
    else
        lightTheme();
}

HomeView::~HomeView(){
    delete livePanel;
    livePanel = nullptr;
}

void HomeView::darkTheme(){
    // Theme
    if (exchange == LUNO_EXCHANGE)
        view->page()->runJavaScript( R"javascript(
                                    tvWidget.changeTheme("Dark");
                                    )javascript");
    
    workPanel->darkTheme();
    livePanel->darkTheme();
    
}
void HomeView::lightTheme() {
    // Theme
    if (exchange == LUNO_EXCHANGE)
        view->page()->runJavaScript( R"javascript(
                                    tvWidget.changeTheme("Light");
                                    )javascript");
    
    livePanel->lightTheme();
    workPanel->lightTheme();
    
    nightmode = false;
}

void HomeView::updateTheme(){
    //Theme
    if (nightmode && !isDarkMode())
        lightTheme();
    if (!nightmode && isDarkMode())
        darkTheme();
}

void HomeView::forceDarkTheme(){
    darkTheme();
}
