#ifndef Window_hpp
#define Window_hpp

#include <QTextEdit>
#include <QTextBrowser>

#include "OrderPanel.hpp"
#include "ChartPanel.hpp"

QTextEdit& operator<< (QTextEdit& stream, std::string str);
QTextBrowser& operator<< (QTextBrowser& stream, std::string str);

class HomeWindow {
public:
    HomeWindow (QWidget *parent = 0);
    
    QTextEdit* text;
    OrderPanel *orderPanel;
    ChartPanel *chartPanel;
};

#endif /* Window_hpp */
