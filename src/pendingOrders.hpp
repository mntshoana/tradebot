#ifndef OPEN_ORDER_PANEL_HPP
#define OPEN_ORDER_PANEL_HPP
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QDateTime>

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

#include "lunoclient.hpp"
#include "textPanel.hpp"

class PendingOrdersPanel : public QWidget {
    Q_OBJECT
private:
    std::vector<std::string> orderIds;
    TextPanel text;
    
public:
    QVBoxLayout *format;
    QHBoxLayout *line;
    std::vector<Luno::UserOrder> openUserOrders;
    
    void clearItems();
    void createItem (Luno::UserOrder&);
    void createTitle ();
    void addOrders ();
    PendingOrdersPanel(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent *);
    
    void popFrontOrder();
};

#endif
