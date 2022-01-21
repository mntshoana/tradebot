#include "balancePanel.hpp"

BalancePanel::BalancePanel(QWidget* parent) : QWidget(parent) {
    setObjectName("UserBalances");
    
    TextPanel::init(parent);
    text = TextPanel::textPanel;
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
    
    lblAsset->setFixedHeight(15);lblAsset->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblBalance->setFixedHeight(15);lblBalance->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblReserved->setFixedHeight(15);lblReserved->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblUncomfirmed->setFixedHeight(15);lblUncomfirmed->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblAccountID->setFixedHeight(15);lblAccountID->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    
    line = new QHBoxLayout;
    line->addWidget( lblAsset,3);
    line->addWidget( lblBalance,8, Qt::AlignCenter);
    line->addWidget( lblReserved,8, Qt::AlignCenter);
    line->addWidget( lblUncomfirmed,8, Qt::AlignCenter);
    line->addWidget( new QLabel(""), 2);
    line->addWidget( lblAccountID,8, Qt::AlignCenter);
    line->setContentsMargins(1,0,0,12);
    line->setAlignment(Qt::AlignTop);
    format->addLayout(line);
}

struct SepFacet : std::numpunct<char> {
   /* use space as separator */
   char do_thousands_sep() const { return ' '; }

   /* digits are grouped by 3 digits each */
   std::string do_grouping() const { return "\3"; }
};

std::string BalancePanel::floatToString(float val, const int decimals )
{
    std::ostringstream out;
    out.imbue(std::locale(std::locale(), new SepFacet));
    out.precision(decimals);
    out << std::fixed << val;
    return out.str();
}

void BalancePanel::createItem (BalanceType& balance)
{
    QFont big( "Helvetica", 15, QFont::Normal);
    QFont normal( "Helvetica", 13, QFont::Normal);
    
    QLabel *lblAsset = new QLabel ( QString::fromStdString(balance.getAsset() ) );
    lblAsset->setFont(big);
    
    bool isZar = balance.getAsset() == "ZAR";
    std::string prefixR = (isZar ? "R " : "");
    QLabel *lblBalance = new QLabel ( QString::fromStdString(prefixR + floatToString(balance.getBalance(), isZar ? 2 : 6 )) );
    lblBalance->setFont(normal);
    
    QLabel *lblReserved = new QLabel ( QString::fromStdString(prefixR +floatToString(balance.getReserved(), isZar ? 2 : 6 ) ) );
    lblReserved->setFont(normal);
    QLabel *lblUncomfirmed = new QLabel ( QString::fromStdString(prefixR +floatToString(balance.getUncomfirmed(), isZar ? 2 : 6 ) ) );
    lblUncomfirmed->setFont(normal);
    
    QLabel *lblAccountID = new QLabel ( QString::fromStdString(balance.getAccountID() ) );
    lblAccountID->setFont(normal);
    

    
    lblAsset->setMinimumHeight(15);lblAsset->setFixedHeight(15);lblAsset->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblBalance->setMinimumHeight(15);lblBalance->setFixedHeight(15);lblBalance->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblReserved->setMinimumHeight(15);lblReserved->setFixedHeight(15);lblReserved->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblUncomfirmed->setMinimumHeight(15);lblUncomfirmed->setFixedHeight(15);lblUncomfirmed->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblAccountID->setMinimumHeight(15);lblAccountID->setFixedHeight(15);lblAccountID->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    
    // add the label and button to the layout
    line = new QHBoxLayout;
    line->addWidget( lblAsset,3);
    line->addWidget( lblBalance,8, Qt::AlignCenter);
    line->addWidget( lblReserved,8, Qt::AlignCenter);
    line->addWidget( lblUncomfirmed,8, Qt::AlignCenter);
    line->addWidget( new QLabel(""), 2);
    line->addWidget( lblAccountID,8, Qt::AlignCenter);
    line->setContentsMargins(1,10,0,0);
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
            *text << ex.String().c_str(); // To do:: should be an error stream here
    }
}

void BalancePanel::paintEvent(QPaintEvent *) {
     // The following allows Qt style sheets to work on a derived class (#derivedClassName)
     QStyleOption options;
     options.initFrom(this);
     QPainter painter(this);
     style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
    
}
