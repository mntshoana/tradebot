#ifndef Window_hpp
#define Window_hpp

#include <thread>
#include <QWebEngineView>
#include "livePanel.hpp"
#include "workspacePanel.hpp"


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
    
    int exchange;
public:
    HomeView (QWidget *parent = nullptr, int exchange = LUNO_EXCHANGE);
    virtual ~HomeView();
    
    QWebEngineView *view;
    LivePanel *livePanel;
    WorkspacePanel *workPanel;
    
    std::vector<Luno::Trade> ticks, moreticks;
    
    virtual void updateTheme();
    virtual void forceDarkTheme();
};
#endif /* Window_hpp */
