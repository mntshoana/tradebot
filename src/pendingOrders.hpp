#ifndef OPEN_ORDER_PANEL_HPP
#define OPEN_ORDER_PANEL_HPP
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QDateTime>

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

#include <chrono>
#include <thread>

#include "client/lunoclient/lunoclient.hpp"
#include "client/valrclient/valrclient.hpp"
#include "textPanel.hpp"

class PendingOrdersPanel : public QWidget {
    Q_OBJECT
private:
    std::vector<std::string> orderIds;
    TextPanel* text;
    
public:
    QVBoxLayout *format;
    QHBoxLayout *line;
    bool criticalUpdate;
    
    void clearItems();
    void createItem (OrderType&);
    void createTitle ();
    void addOrders (std::vector<OrderType*>* openUserOrders);
    PendingOrdersPanel(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent *);
    
    void popFrontOrder();
};

#endif
