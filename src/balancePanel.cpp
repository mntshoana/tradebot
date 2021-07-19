#include "balancePanel.hpp"

BalancePanel::BalancePanel(QWidget* parent) : QWidget(parent) {
    setGeometry(0, 500, 930, 220);
    
    format = new QVBoxLayout;
    format->setSpacing(0);
    format->setAlignment(Qt::AlignTop);
    setLayout(format);
    
    createTitle();
    loadItems();
}

void BalancePanel::reloadItems(){
    for  (int i = accountIDs.size(); i >= 0; i--){
        QLayout *level = format->takeAt(i)->layout();
        while(!level->isEmpty()) {
            QWidget *w = level->takeAt(0)->widget();
            delete w;
        }
        delete level;
    }
    accountIDs.clear();
    createTitle();
    loadItems();
}

void BalancePanel::createTitle (){
    QLabel *lblAsset = new QLabel ( QString::fromStdString("Asset" ) );
    QLabel *lblBalance = new QLabel ( QString::fromStdString("Balance" ) );
    QLabel *lblReserved = new QLabel ( QString::fromStdString("Reserved") );
    QLabel *lblUncomfirmed = new QLabel ( QString::fromStdString("Uncomfirmed" ) );
    QLabel *lblAccountID = new QLabel ( QString::fromStdString("AccountID" ) );
    
    lblAsset->setFixedHeight(10);lblAsset->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblBalance->setFixedHeight(10);lblBalance->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblReserved->setFixedHeight(10);lblReserved->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblUncomfirmed->setFixedHeight(10);lblUncomfirmed->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblAccountID->setFixedHeight(10);lblAccountID->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    
    line = new QHBoxLayout;
    line->addWidget( lblAsset,3);
    line->addWidget( lblBalance,4);
    line->addWidget( lblReserved,4);
    line->addWidget( lblUncomfirmed,4);
    line->addWidget( lblAccountID,5);
    line->setContentsMargins(1,0,0,15);
    line->setAlignment(Qt::AlignTop);
    format->addLayout(line);
}

std::string BalancePanel::floatToString(float val, const int decimals )
{
    std::ostringstream out;
    out.precision(decimals);
    out << std::fixed << val;
    return out.str();
}

void BalancePanel::createItem (Luno::Balance& balance)
{
    QLabel *lblAsset = new QLabel ( QString::fromStdString(balance.asset ) );
    bool isZar = balance.asset == "ZAR";
    QLabel *lblBalance = new QLabel ( QString::fromStdString(floatToString(balance.balance, isZar ? 2 : 6 )) );
    
    QLabel *lblReserved = new QLabel ( QString::fromStdString(floatToString(balance.reserved, isZar ? 2 : 6 ) ) );
    QLabel *lblUncomfirmed = new QLabel ( QString::fromStdString(floatToString(balance.uncomfirmed, isZar ? 2 : 6 ) ) );
    
    QLabel *lblAccountID = new QLabel ( QString::fromStdString(balance.accountID ) );
    
    

    
    lblAsset->setMinimumHeight(15);lblAsset->setFixedHeight(15);lblAsset->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblBalance->setMinimumHeight(15);lblBalance->setFixedHeight(15);lblBalance->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblReserved->setMinimumHeight(15);lblReserved->setFixedHeight(15);lblReserved->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblUncomfirmed->setMinimumHeight(15);lblUncomfirmed->setFixedHeight(15);lblUncomfirmed->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblAccountID->setMinimumHeight(15);lblAccountID->setFixedHeight(15);lblAccountID->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    
    // add the label and button to the layout
    line = new QHBoxLayout;
    line->addWidget( lblAsset,3);
    line->addWidget( lblBalance,4);
    line->addWidget( lblReserved,4);
    line->addWidget( lblUncomfirmed,4);
    line->addWidget( lblAccountID,5);
    line->setContentsMargins(1,0,0,0);
    line->setAlignment(Qt::AlignTop);
    //line->sizeHint()
    
    // minimumSize
    //maximumSize properties.
    line->setGeometry(QRect(0, 500, 930, 30));
    line->addStrut(10);
    
    format->addLayout(line);
    
}

void BalancePanel::loadItems (){
    try{
        userBalances = Luno::LunoClient::getBalances();
        for (Luno::Balance& balance : userBalances){
                accountIDs.push_back(balance.accountID);
                createItem( balance );
        }
        
    } catch (ResponseEx ex){
            text << ex.String().c_str(); // To do:: should be an error stream here
    }
}

void BalancePanel::paintEvent(QPaintEvent *) {
     // The following allows Qt style sheets to work on a derived class (#derivedClassName)
     QStyleOption options;
     options.initFrom(this);
     QPainter painter(this);
     style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
    
}
