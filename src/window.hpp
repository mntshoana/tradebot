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
    
    virtual void loadLocalTicks() {/*does nothing*/};
    virtual std::string lastTrades() { return "This is just a HomeView template!";}
    virtual Task* toUpdateOrderBook() { return new Task( []() {/*does nothing*/});}
    virtual Task* toDownloadTicks(std::string pair = "DEFAULT") { return new Task( []() {/*does nothing*/});}
        
};

class LunoHomeView : public HomeView {
protected:
    virtual void darkTheme() override;
    virtual void lightTheme() override;
    
public:
    LunoHomeView (QWidget *parent = nullptr);
    virtual ~LunoHomeView();
    
    std::vector<Luno::Trade> ticks, moreticks;
    
    virtual void loadLocalTicks() override;
    void downloadTicks(std::string pair);
    virtual std::string lastTrades() override;
    
    virtual Task* toUpdateOrderBook() override;
    virtual Task* toDownloadTicks(std::string pair = "XBTZAR") override;
};

class VALRHomeView : public HomeView {
private:
    virtual void darkTheme() override;
    virtual void lightTheme() override;
    
public:
    VALRHomeView (QWidget *parent = nullptr);
    virtual ~VALRHomeView();
    
    std::vector<VALR::Trade> ticks, moreticks;
    
    virtual Task* toUpdateOrderBook() override;
    virtual Task* toDownloadTicks(std::string pair = "BTCZAR") override;
};
#endif /* Window_hpp */
