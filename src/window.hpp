#ifndef Window_hpp
#define Window_hpp

#include <thread>
#include <QWebEngineView>
#include "orderPanel.hpp"
#include "pendingOrders.hpp"
#include "balancePanel.hpp"
#include "withdrawPanel.hpp"
#include "textPanel.hpp"


QTextEdit& operator<< (QTextEdit& stream, std::string str);
QTextBrowser& operator<< (QTextBrowser& stream, std::string str);

class Window {
private:
    virtual void darkTheme() = 0;
    virtual void lightTheme() = 0;
public:
    virtual void updateTheme() = 0;
    virtual void forceDarkTheme() = 0;
    bool nightmode;
};

class HomeView : public Window, public QWidget {
private:
    virtual void darkTheme();
    virtual void lightTheme();
public:
    HomeView (QWidget *parent = 0);
    virtual ~HomeView();
    
    QTabWidget *tabWidget;
    // Tabs will have
    TextPanel text;
    PendingOrdersPanel *pendingOrders;
    BalancePanel *userBalances;
    WithdrawPanel *withdrawals;
    ///
    OrderPanel *orderPanel;
    
    QWebEngineView *view;
    
    
    Luno::LunoClient *lunoClient;
    std::vector<Luno::Trade> ticks, moreticks;
    
    virtual void updateTheme();
    virtual void forceDarkTheme();
};
#endif /* Window_hpp */
