#include "pendingOrders.hpp"

PendingOrdersPanel::PendingOrdersPanel(QWidget* parent) : QWidget(parent) {
    setGeometry(0, 500, 930, 220);
    
    format = new QVBoxLayout;
    format->setSpacing(0);
    format->setAlignment(Qt::AlignTop);
    setLayout(format);
    
    createTitle();
    addOrders();
}

void PendingOrdersPanel::clearItems(){
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
    addOrders();
}

void PendingOrdersPanel::createTitle (){
    QLabel *lblDate = new QLabel ( QString::fromStdString("Created" ) );
    QLabel *lblType = new QLabel ( QString::fromStdString("Type" ) );
    QLabel *lblPrice = new QLabel ( QString::fromStdString("Price") );
    QLabel *lblVolume = new QLabel ( QString::fromStdString("Volume" ) );
    QLabel *lblValue = new QLabel ( QString::fromStdString("Value" ) );
    QLabel *lblFill = new QLabel ( QString::fromStdString("Fill" ) );
    
    lblDate->setFixedHeight(15);lblDate->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblType->setFixedHeight(15);lblType->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblPrice->setFixedHeight(15);lblPrice->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblVolume->setFixedHeight(15);lblVolume->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblValue->setFixedHeight(15);lblValue->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblFill->setFixedHeight(15); lblFill->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    
    line = new QHBoxLayout;
    line->addWidget( lblDate,5);
    line->addWidget( lblType,2);
    line->addWidget( lblPrice,3);
    line->addWidget( lblVolume,3);
    line->addWidget( lblValue,3);
    line->addWidget( lblFill,4, Qt::AlignLeft);
    line->setContentsMargins(1,0,0,15);
    line->setAlignment(Qt::AlignTop);
    format->addLayout(line);
}

void PendingOrdersPanel::createItem (Luno::UserOrder& order)
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
    
    lblDate->setMinimumHeight(15);lblDate->setFixedHeight(15);lblDate->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblType->setMinimumHeight(15);lblType->setFixedHeight(15);lblType->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblPrice->setMinimumHeight(15);lblPrice->setFixedHeight(15);lblPrice->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblVolume->setMinimumHeight(15);lblVolume->setFixedHeight(15);lblVolume->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblValue->setMinimumHeight(15);lblValue->setFixedHeight(15);lblValue->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblFill->setMinimumHeight(15);lblFill->setFixedHeight(15); lblFill->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    but->setMinimumHeight(15);but->setFixedHeight(15); but->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

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
    //line->sizeHint()
    
    // minimumSize
    //maximumSize properties.
    line->setGeometry(QRect(0, 500, 930, 30));
    line->addStrut(10);
    
    format->addLayout(line);

    connect(but, &QPushButton::clicked, this, [this, order] () {
        auto it = std::find(orderIds.begin(), orderIds.end(), order.orderID);
        int index = it - orderIds.begin() + 1; // plus title row
        std::string result = Luno::LunoClient::stopOrder(order.orderID);
        
        QLayout *level = format->takeAt(index)->layout();
        while(!level->isEmpty()) {
            QWidget *w = level->takeAt(0)->widget();
            delete w;
        }
        delete level;
        
        orderIds.erase(it);
        
        // output
        text << "Cancelled order!";
        text << ("COMPLETE: " + result).c_str();
    });
    
}

void PendingOrdersPanel::addOrders (){
    try{
        openUserOrders = Luno::LunoClient::getUserOrders("XBTZAR", "PENDING");
        for (Luno::UserOrder& order : openUserOrders){
            bool exists = false;
            for (std::string& id : orderIds){
                if (order.orderID == id){
                    exists = true;
                    break;
                }
            }
            
            if (!exists){
                orderIds.push_back(order.orderID);
                createItem( order);
            }
        }
    } catch (ResponseEx ex){
            text << ex.String().c_str(); // To do:: should be an error stream here
    }
}

void PendingOrdersPanel::paintEvent(QPaintEvent *)
 {
     // The following allows Qt style sheets to work on a derived class (#derivedClassName)
     QStyleOption options;
     options.initFrom(this);
     QPainter painter(this);
     style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
 }

void PendingOrdersPanel::popFrontOrder(){
    QList<QHBoxLayout *> list = format->findChildren<QHBoxLayout *> ();
    int displayedOrderListCount = list.size();
    displayedOrderListCount--; // Must exclude the title row
    if (orderIds.size() > 0 && orderIds.size() == displayedOrderListCount){
        std::string result = Luno::LunoClient::stopOrder(orderIds[0]);
        
        QLayout *level = format->takeAt(1)->layout();
        while(!level->isEmpty()) {
            QWidget *w = level->takeAt(0)->widget();
            delete w;
        }
            
        delete level;

        orderIds.erase(orderIds.begin());
        
        // output
        text << "Cancelled order!";
        text << ("COMPLETE: " + result).c_str();
    }
}