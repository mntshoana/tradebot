#ifndef Window_hpp
#define Window_hpp

#include <QTextEdit>
#include <QTextBrowser>

#include "OrderPanel.hpp"
#include "ChartPanel.hpp"

QTextEdit& operator<< (QTextEdit& stream, std::string str);
QTextBrowser& operator<< (QTextBrowser& stream, std::string str);

class Window {
public:
    virtual void darkTheme() = 0;
    virtual void lightTheme() = 0;
    
    bool nightmode;
};

class HomeWindow : public Window {
public:
    HomeWindow (QWidget *parent = 0);
    
    QTextEdit* text;
    OrderPanel *orderPanel;
    ChartPanel *chartPanel;
    
    virtual void darkTheme();
    virtual void lightTheme();
};

#endif /* Window_hpp */
