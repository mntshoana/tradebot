#include "window.hpp"
#include <QTimer>


QTextEdit& operator<< (QTextEdit& stream, std::string str) {
    stream.append(str.c_str());
    return stream;
}
QTextBrowser& operator<< (QTextBrowser& stream, std::string str){
    stream.append(str.c_str());
    return stream;
}

HomeView::HomeView (QWidget *parent) : QWidget(parent) {
    TextPanel::init(parent);
    
    livePanel = new LivePanel(parent);

    // Chart Displayed using QWebEngine browser
    view = new QWebEngineView(parent); // to do: move VIEW to a chart widget
    view->setGeometry(0, 0, 930, 500);
    view->load(QUrl("https://d32exi8v9av3ux.cloudfront.net/static/scripts/tradingview.prod.html?symbol=XBTZAR&res=60&lang=en"));
    
    workPanel = new WorkspacePanel(parent);
    workPanel->setGeometry(0, 472, 930, 248);
    
    
    // Note: the next line only works if the browser has already loaded, hence delay
    if (!isDarkMode())
        QTimer::singleShot(4500, this, [this](){
                view->page()->runJavaScript( R"java(
                tvWidget.changeTheme("Light");
                bgcolor(white);
            )java");
        });
    
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
    view->page()->runJavaScript( R"javascript(
                                    tvWidget.changeTheme("Dark");
                                )javascript");
    
    workPanel->darkTheme();
    livePanel->darkTheme();
    
}
void HomeView::lightTheme() {
    // Theme
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
