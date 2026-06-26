#ifndef TradeBot_hpp
#define TradeBot_hpp

#include <QApplication>
#include <QPalette>

#include <QtCore/QDateTime>

#include "objectivec.h"

#include "window.hpp"

#include <QWebSocket>
#include <QTimer>
#include <QJsonDocument>

class TradeBot : public QWidget
{
    Q_OBJECT
public:
    TradeBot ();
    
    static TradeBot* staticThis;
    void updateExchange(int exchange);
    
private:
    void updateInterval(const std::string& period);
    
private:
    std::thread thread;
    
    HomeView* home;
    
    QWebSocket*  ws            = nullptr;
    QTimer*      displayTimer  = nullptr;
    QTimer*      balanceTimer  = nullptr;
    QTimer*      ordersTimer   = nullptr;
    QTimer*      themeTimer    = nullptr;
    
    void connectWebSocket();
    void setupTimers();
    
    bool eventFilter(QObject *obj, QEvent *event)override;
    
    public slots:
    void onEnqueueUserOrder(std::string orderID);
    
    private slots:
    void onWsMessage(const QString& msg);
    void cleanup();

};
#endif /* TradeBot_hpp */
