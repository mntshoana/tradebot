#ifndef Window_hpp
#define Window_hpp

#include <QTextEdit>
#include <QTextBrowser>

#include "OrderPanel.hpp"
#include "ChartPanel.hpp"

QTextEdit& operator<< (QTextEdit& stream, std::string str);
QTextBrowser& operator<< (QTextBrowser& stream, std::string str);

class Window {
private:
    virtual void darkTheme() = 0;
    virtual void lightTheme() = 0;
public:
    virtual void updateTheme() = 0;
    
    bool nightmode;
};

class HomeView : public Window {
private:
    virtual void darkTheme();
    virtual void lightTheme();
public:
    HomeView (QWidget *parent = 0);
    ~HomeView();
    
    QTextEdit* text;
    OrderPanel *orderPanel;
    ChartPanel *chartPanel;

    virtual void updateTheme();
};

class P2PView : public Window {
private:
    virtual void darkTheme();
    virtual void lightTheme();
    
public:
    P2PView (QWidget *parent = 0);
    
    QTextEdit* text;
    
    virtual void updateTheme();
};

#endif /* Window_hpp */
