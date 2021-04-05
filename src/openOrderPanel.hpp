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
public:
    QVBoxLayout *format;
    QHBoxLayout *line;
    
    void CreateItem (Luno::UserOrder&, Luno::LunoClient* client);
    void AddItem (std::vector<Luno::UserOrder>& openOrders, Luno::LunoClient* client);
    OpenOrderPanel(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent *);
};

#endif
