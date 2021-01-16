#ifndef Window_hpp
#define Window_hpp


#include "orderPanel.hpp"
#include "chartPanel.hpp"

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
    ~P2PView();
    
    QTextEdit* text;
    
    virtual void updateTheme();
};

#endif /* Window_hpp */
