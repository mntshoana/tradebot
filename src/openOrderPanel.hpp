#ifndef OPEN_ORDER_PANEL_HPP
#define OPEN_ORDER_PANEL_HPP
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QTextStream>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

#include <QScrollArea>
#include <QDateTime>
#include "lunoclient.hpp"

#include <sstream>
class OpenOrderPanel : public QWidget {
    Q_OBJECT
private:
    Luno::LunoClient* client;
    std::vector<std::string> orderIds;
    bool hasTitle;
    
public:
    QVBoxLayout *format;
    QHBoxLayout *line;
    
    void clearItems();
    void createItem (Luno::UserOrder&);
    void createTitle ();
    void addOrders ();
    OpenOrderPanel(QWidget* parent = nullptr, Luno::LunoClient* client = nullptr);
    void paintEvent(QPaintEvent *);
    
    void popFrontOrder(QTextEdit* text);
};

#endif
