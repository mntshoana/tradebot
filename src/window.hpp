#ifndef Window_hpp
#define Window_hpp

#include <thread>
#include <QWebEngineView>
#include "livePanel.hpp"
#include "workspacePanel.hpp"
#include <QJsonObject>
#include <QJsonArray>

QTextEdit& operator<< (QTextEdit& stream, std::string str);
QTextBrowser& operator<< (QTextBrowser& stream, std::string str);

class Window {
protected:
    virtual void darkTheme() = 0;
    virtual void lightTheme() = 0;
public:
    virtual void updateTheme() = 0;
    virtual void forceDarkTheme() = 0;
    bool nightmode;
};

class HomeView : public Window, public QWidget {
protected:
    virtual void darkTheme();
    virtual void lightTheme();
    
    static int exchange;
    std::fstream file;
    std::string path;
    unsigned long long *timestamp; /*milliseconds sinc epoch*/
public:
    HomeView (int exchange, QWidget *parent = nullptr);
    virtual ~HomeView();
    
    QWebEngineView *view;
    LivePanel *livePanel;
    WorkspacePanel *workPanel;
    
    
    bool closing;
    
    virtual void updateTheme();
    virtual void forceDarkTheme();
    
    static int getExchangeVal(){ return exchange; }
    static std::string getTickFileName();
    
    virtual void loadLocalTicks(std::string = "DEFAULT") {/*does nothing*/};
    virtual std::string lastTrades() { return "This is just a HomeView template!";}
    virtual void onOrderBook(const QJsonObject& /*data*/) {}
    virtual void onTrades(const QJsonArray& /*trades*/) {}
    virtual void refreshOpenOrders() {}
        
};

class LunoHomeView : public HomeView {
private:
    std::vector<Luno::Trade> ticks, moreticks;
    std::vector<Luno::UserOrder> lunoOrders;
protected:
    virtual void darkTheme() override;
    virtual void lightTheme() override;
    
public:
    LunoHomeView (QWidget *parent = nullptr);
    virtual ~LunoHomeView();
    
    virtual void loadLocalTicks(std::string pair) override;
    virtual std::string lastTrades() override;
    virtual void onOrderBook(const QJsonObject& data) override;
    virtual void onTrades(const QJsonArray& trades) override;
    virtual void refreshOpenOrders() override;
};

class VALRHomeView : public HomeView {
private:
    std::vector<VALR::Trade> ticks, moreticks;
    std::vector<VALR::UserOrder> valrOrders;
protected:
    virtual void darkTheme() override;
    virtual void lightTheme() override;
    
public:
    VALRHomeView (QWidget *parent = nullptr);
    virtual ~VALRHomeView();
    
    virtual void loadLocalTicks(std::string pair) override;
    virtual std::string lastTrades() override;
    virtual void onOrderBook(const QJsonObject& data) override;
    virtual void onTrades(const QJsonArray& trades) override;
};
#endif /* Window_hpp */
