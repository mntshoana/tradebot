#ifndef OPEN_ORDER_PANEL_HPP
#define OPEN_ORDER_PANEL_HPP
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QTextStream>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

#include <QDateTime>
#include "lunoclient.hpp"

#include <sstream>
class OpenOrderPanel : public QWidget {
    Q_OBJECT
private:
    std::vector<std::string> orderIds;
    bool hasTitle;
public:
    QVBoxLayout *format;
    QHBoxLayout *line;
    
    void clearItems();
    void createItem (Luno::UserOrder&, Luno::LunoClient* client);
    void createTitle ();
    void addItem (std::vector<Luno::UserOrder>& openOrders, Luno::LunoClient* client);
    OpenOrderPanel(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent *);
};

#endif
