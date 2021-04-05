#include "openOrderPanel.hpp"

OpenOrderPanel::OpenOrderPanel(QWidget* parent) : QWidget(parent){
    setGeometry(0, 500, 1180, 220);
    
    format = new QVBoxLayout();
    format->setSpacing(0);
    setLayout(format);
    
    
}
void OpenOrderPanel::CreateItem (Luno::UserOrder& order, Luno::LunoClient* client )
{
    std::stringstream ss;
    ss << "Created: ";
    ss << QDateTime::fromMSecsSinceEpoch(order.createdTime).toString("ddd MMMM d yyyy hh:mm").toStdString();
    ss << " Price:" << order.price;
    
    QLabel *lab = new QLabel ( QString::fromStdString(ss.str() ) );
    QPushButton *but = new QPushButton ( "Cancel" );
    
    // add the label and button to the layout
    line = new QHBoxLayout;
    line->addWidget( lab,19);
    line->addWidget( but,1 );
    line->setContentsMargins(1,0,0,0);
    
    format->addLayout(line);

    connect(but, &QPushButton::clicked, this, [this, order, client] () {
        auto it = std::find(orderIds.begin(), orderIds.end(), order.orderID);
        int index = it - orderIds.begin();
        client->stopOrder(order.orderID);
        
        QLayout *level = format->takeAt(index)->layout();
        while(!level->isEmpty()) {
            QWidget *w = level->takeAt(0)->widget();
            delete w;
        }
        delete level;
        
        orderIds.erase(it);
    });
    
}

void OpenOrderPanel::AddItem (std::vector<Luno::UserOrder>& openOrders, Luno::LunoClient* client){
    for (Luno::UserOrder& order : openOrders){
        bool exists = false;
        for (std::string& id : orderIds){
            if (order.orderID == id){
                exists = true;
                break;
            }
        }
        
        if (!exists){
            orderIds.push_back(order.orderID);
            CreateItem( order, client );
        }
    }
}

void OpenOrderPanel::paintEvent(QPaintEvent *)
 {
     // The following allows Qt style sheets to work on a derived class (#derivedClassName)
     QStyleOption options;
     options.initFrom(this);
     QPainter painter(this);
     style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
 }
