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

class HomeWindow : public Window {
private:
    virtual void darkTheme();
    virtual void lightTheme();
public:
    HomeWindow (QWidget *parent = 0);
    
    QTextEdit* text;
    OrderPanel *orderPanel;
    ChartPanel *chartPanel;

    virtual void updateTheme();
};

#endif /* Window_hpp */
