#include "openOrderPanel.hpp"

OpenOrderPanel::OpenOrderPanel(QWidget* parent) : QWidget(parent){
    setGeometry(0, 500, 1180, 220);
    
    format = new QVBoxLayout();
    format->setSpacing(0);
    
    setLayout(format);
    
    createTitle();
}

void OpenOrderPanel::clearItems(){
    for  (int i = orderIds.size(); i >= 0; i--){
        QLayout *level = format->takeAt(i)->layout();
        while(!level->isEmpty()) {
            QWidget *w = level->takeAt(0)->widget();
            delete w;
        }
        delete level;
    }
    orderIds.clear();
    createTitle();
}

void OpenOrderPanel::createTitle (){
    QLabel *lblDate = new QLabel ( QString::fromStdString("Created" ) );
    QLabel *lblType = new QLabel ( QString::fromStdString("Type" ) );
    QLabel *lblPrice = new QLabel ( QString::fromStdString("Price") );
    QLabel *lblVolume = new QLabel ( QString::fromStdString("Volume" ) );
    QLabel *lblValue = new QLabel ( QString::fromStdString("Value" ) );
    QLabel *lblFill = new QLabel ( QString::fromStdString("Fill" ) );
    line = new QHBoxLayout;
    line->addWidget( lblDate,5);
    line->addWidget( lblType,2);
    line->addWidget( lblPrice,3);
    line->addWidget( lblVolume,3);
    line->addWidget( lblValue,3);
    line->addWidget( lblFill,4, Qt::AlignLeft);
    line->setContentsMargins(1,0,0,0);
    line->setAlignment(Qt::AlignTop);
    
    format->addLayout(line);
    hasTitle = true;
}

void OpenOrderPanel::createItem (Luno::UserOrder& order, Luno::LunoClient* client )
{
    QLabel *lblDate = new QLabel ( QDateTime::fromMSecsSinceEpoch(order.createdTime).toString("ddd MMMM d yyyy hh:mm") );
    QLabel *lblType = new QLabel ( QString::fromStdString(order.type ) );
    QLabel *lblPrice = new QLabel ( QString::fromStdString(std::to_string(order.price) ) );
    QLabel *lblVolume = new QLabel ( QString::fromStdString(std::to_string(order.volume) ) );
    
    std::stringstream ss;
    ss << " ( at R" << (std::trunc(order.price * order.volume * 100) / 100) << ")";
    QLabel *lblValue = new QLabel ( QString::fromStdString(ss.str() ) );
    
    ss.str("");
    ss << order.baseValue << " [" << order.baseValue / order.volume * 100 << "%]";
    QLabel *lblFill = new QLabel ( QString::fromStdString(ss.str() ) );
    
    QPushButton *but = new QPushButton ( "Cancel" );
    
    // add the label and button to the layout
    line = new QHBoxLayout;
    line->addWidget( lblDate,5);
    line->addWidget( lblType,2);
    line->addWidget( lblPrice,3);
    line->addWidget( lblVolume,3);
    line->addWidget( lblValue,3);
    line->addWidget( lblFill,3);
    line->addWidget( but,1 );
    line->setContentsMargins(1,0,0,0);
    line->setAlignment(Qt::AlignTop);
    
    format->addLayout(line);

    connect(but, &QPushButton::clicked, this, [this, order, client] () {
        auto it = std::find(orderIds.begin(), orderIds.end(), order.orderID);
        int index = it - orderIds.begin() + 1; // plus title row
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

void OpenOrderPanel::addItem (std::vector<Luno::UserOrder>& openOrders, Luno::LunoClient* client){
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
            createItem( order, client );
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
