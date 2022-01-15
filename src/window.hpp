#ifndef Window_hpp
#define Window_hpp

#include <thread>
#include <QWebEngineView>
#include "livePanel.hpp"
#include "workspacePanel.hpp"
#include "job.hpp"

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
    
    int exchange;
    std::fstream file;
    std::string path;
    unsigned long long *timestamp;
public:
    HomeView (QWidget *parent = nullptr, int exchange = LUNO_EXCHANGE);
    virtual ~HomeView();
    
    QWebEngineView *view;
    LivePanel *livePanel;
    WorkspacePanel *workPanel;
    
    
    bool closing;
    
    virtual void updateTheme();
    virtual void forceDarkTheme();
    
    virtual void loadLocalTicks(std::string = "DEFAULT") {/*does nothing*/};
    virtual std::string lastTrades() { return "This is just a HomeView template!";}
    virtual Task* toUpdateOrderBook(std::string = "DEFAULT") { return new Task( []() {/*does nothing*/});}
    virtual Task* toUpdateOpenUserOrders() { return new Task( []() {/*does nothing*/});}
    virtual Task* toAppendOpenUserOrder(std::string ) { return new Task( []() {/*does nothing*/});}
    virtual Task* toDownloadTicks(std::string = "DEFAULT") { return new Task( []() {/*does nothing*/});}
        
};

class LunoHomeView : public HomeView {
private:
    void downloadTicks(std::string pair);
    
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
    
    virtual Task* toUpdateOrderBook(std::string pair) override;
    virtual Task* toUpdateOpenUserOrders() override;
    virtual Task* toAppendOpenUserOrder(std::string orderID) override;
    virtual Task* toDownloadTicks(std::string pair) override;
};

class VALRHomeView : public HomeView {
private:
    void downloadTicks(std::string pair);
    
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
    
    virtual Task* toUpdateOrderBook(std::string pair) override;
    virtual Task* toUpdateOpenUserOrders() override;
    virtual Task* toAppendOpenUserOrder(std::string orderID) override;
    virtual Task* toDownloadTicks(std::string pair) override;
};
#endif /* Window_hpp */
