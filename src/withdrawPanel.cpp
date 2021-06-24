#include "withdrawPanel.hpp"

WithdrawPanel::WithdrawPanel(QWidget* parent) : QWidget(parent) {
    setGeometry(0, 500, 930, 220);
    
    loadItems();
    
    lblAsset = new QLabel("Asset", this);
    
    QStringList assetList;
    for (Luno::Balance bal : userBalances)
        assetList << QString::fromStdString(bal.asset);
    assetBox = new QComboBox(this);
    assetBox->addItems(assetList);
    assetBox->setCurrentIndex(0);
    
    lblAmount = new QLabel("Amount", this);

    withdraw = new QPushButton("Withdraw", this);
    
    
    txtAmount = new LineBlock(this, withdraw);
    
    lblBalance = new QLabel("", this);
    
    lblInstantWithdrawal = new QLabel("Instant Withdrawal", this);
    cbxFastWithdraw = new QCheckBox(this);
    
    lblPending = new QLabel("Pending withdrawals", this);
    pending = new Pending(this);
    
    
    panelLayout = new QGridLayout;
    boundingBox = new QGroupBox(this);
    panelLayout->addWidget(lblAsset, 1, 1);
    panelLayout->addWidget(assetBox, 1, 2, 1, 2);
    panelLayout->addWidget(lblAmount, 2, 1);
    panelLayout->addWidget(txtAmount, 2, 2, 1, 2);
    panelLayout->addWidget(lblBalance, 2, 4, 1, 1);
    panelLayout->addWidget(lblInstantWithdrawal, 3, 1);
    panelLayout->addWidget(cbxFastWithdraw, 3, 2);
    panelLayout->addWidget(withdraw, 4, 2);
    panelLayout->addWidget(lblPending, 1, 5, 1, 4, Qt::AlignRight);
    panelLayout->addWidget(pending, 2, 5, 3, 4);
    panelLayout->setContentsMargins(30, 30, 30, 60);

    boundingBox->setLayout(panelLayout);
    boundingBox->setGeometry(0, 0, 930, 220);
    connect(assetBox, &QComboBox::currentTextChanged, this,
            [this](const QString &selection){
                for (Luno::Balance bal : userBalances){
                    if (bal.asset == selection.toStdString()){
                        bool isZar = (bal.asset == "ZAR");
                        std::string label =floatToString(bal.balance - bal.reserved, ( isZar ? 2 : 6 ));
                        if (isZar)
                            label = "R " + label;
                        lblBalance->setText(QString::fromStdString(label));
                    }
                }
    });
    emit assetBox->currentTextChanged(assetBox->currentText());
    
    connect(withdraw,
            &QPushButton::clicked, this,[this](){
        text << "Withdraw button clicked. To Widthraw " + txtAmount->text().toStdString();
        try {
            bool isFastWithdrawl = cbxFastWithdraw->isChecked();
            float amount = atof(txtAmount->text().toStdString().c_str()); // ZAR
            std::string result = Luno::LunoClient::withdraw(amount, isFastWithdrawl);
            text << result;
        }
        catch (ResponseEx ex){
            text << " [Error] Unable to withdraw! At " + std::string(__FILE__) + ", line: " + std::to_string(__LINE__);
            text << ex.String();
        }
    });
}

struct SepFacet : std::numpunct<char> {
   /* use space as separator */
   char do_thousands_sep() const { return ' '; }

   /* digits are grouped by 3 digits each */
   std::string do_grouping() const { return "\3"; }
};

std::string WithdrawPanel::floatToString(float val, const int decimals )
{
    std::ostringstream out;
    out.imbue(std::locale(std::locale(), new SepFacet));
    out.precision(decimals);
    out << std::fixed << val;
    return out.str();
}


void WithdrawPanel::loadItems (){
    try{
        userBalances = Luno::LunoClient::getBalances();
    } catch (ResponseEx ex){
            text << ex.String().c_str();
    }
}

void WithdrawPanel::reloadItemsUsing(std::vector<Luno::Balance>& toCopy){
    userBalances = toCopy;
    pending->reloadItems();
}
void WithdrawPanel::paintEvent(QPaintEvent *) {
     // The following allows Qt style sheets to work on a derived class (#derivedClassName)
     QStyleOption options;
     options.initFrom(this);
     QPainter painter(this);
     style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
    
}
    
void WithdrawPanel::Pending::reloadItems(){
    for  (int i = userWithdrawals.size(); i >= 0; i--){
        QLayout *level = format->takeAt(i)->layout();
        while(!level->isEmpty()) {
            QWidget *w = level->takeAt(0)->widget();
            delete w;
        }
        delete level;
    }
    userWithdrawals.clear();
    createTitle();
    loadItems();
}

WithdrawPanel::Pending::Pending(QWidget* parent) : QWidget(parent){
    setAutoFillBackground(true);
    
    format = new QVBoxLayout;
    format->setSpacing(0);
    format->setAlignment(Qt::AlignTop);
    setLayout(format);
    
    createTitle();
    loadItems();
}

void WithdrawPanel::Pending::createItem (Luno::Withdrawal& withdrawal){
    QLabel *lblDate = new QLabel ( QDateTime::fromMSecsSinceEpoch(withdrawal.createdTime).toString("ddd d, hh:mm") );
    QLabel *lblType = new QLabel ( QString::fromStdString(withdrawal.currency ) );
    
    std::string amount = floatToString(withdrawal.amount, (withdrawal.currency == "ZAR" ? 2 : 6));
    QLabel *lblAmount = new QLabel ( QString::fromStdString(amount ) );
    std::string fee = floatToString(withdrawal.fee, (withdrawal.currency == "ZAR" ? 2 : 6));
    QLabel *lblFee = new QLabel ( QString::fromStdString(fee ) );
    
    QPushButton *but = new QPushButton ( "Cancel" );
    
    lblDate->setMinimumHeight(15);lblDate->setFixedHeight(15);lblDate->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblType->setMinimumHeight(15);lblType->setFixedHeight(15);lblType->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblAmount->setFixedHeight(15);lblAmount->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblFee->setFixedHeight(15);lblFee->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    // add the label and button to the layout
    line = new QHBoxLayout;
    line->addWidget( lblDate,9);
    line->addWidget( lblType,5);
    line->addWidget( lblAmount,7);
    line->addWidget( lblFee,5);
    line->addWidget( but,1 );
    line->setContentsMargins(1,0,0,0);
    line->setAlignment(Qt::AlignTop);
    
    line->addStrut(10);
    
    format->addLayout(line);

    connect(but, &QPushButton::clicked, this, [this, withdrawal] () {
        int index = 0;
        while (withdrawal.id != userWithdrawals[index].id){
            index ++;
        }
        
        try{
            std::string result = Luno::LunoClient::cancelWithdrawal(std::to_string(withdrawal.id));
            
            QLayout *level = format->takeAt(index+1 /*jump title*/)->layout();
            while(!level->isEmpty()) {
                QWidget *w = level->takeAt(0)->widget();
                delete w;
            }
            delete level;
            
            userWithdrawals.erase(userWithdrawals.begin() + index);
            // output
            text << "Cancelled order!";
            text << ("COMPLETE: " + result).c_str();
        }
        catch (ResponseEx& ex){
            text << " [Error] Unable to cancel withdrawal! at " + std::string(__FILE__) + ", line: " + std::to_string(__LINE__);
            text << ex.String();
        }
    });
}

void WithdrawPanel::Pending::createTitle (){
    QLabel *lblDate = new QLabel ( QString::fromStdString("Created on" ) );
    QLabel *lblType = new QLabel ( QString::fromStdString("Type" ) );

    QLabel *lblAmount = new QLabel ( QString::fromStdString("Amount") );
    QLabel *lblFee = new QLabel ( QString::fromStdString("Fee" ) );
    
    lblDate->setFixedHeight(15);lblDate->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblType->setFixedHeight(15);lblType->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblAmount->setFixedHeight(15);lblAmount->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblFee->setFixedHeight(15);lblFee->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    
    line = new QHBoxLayout;
    line->addWidget( lblDate,8);
    line->addWidget( lblType,4);
    line->addWidget( lblAmount,6);
    line->addWidget( lblFee,9, Qt::AlignLeft);
    line->setContentsMargins(1,0,0,12);
    line->setAlignment(Qt::AlignTop);
    format->addLayout(line);
}

void WithdrawPanel::Pending::loadItems (){
    try {
        std::vector<Luno::Withdrawal> retrieved = Luno::LunoClient::getWithdrawalList();
        for (Luno::Withdrawal& w : retrieved){
            if (w.status == "PENDING"){
                createItem( w );
                userWithdrawals.push_back(w);
            }
        }
    } catch (ResponseEx ex){
            text << ex.String().c_str(); // To do:: should be an error stream here
    }
}
void WithdrawPanel::Pending::paintEvent(QPaintEvent *){
    // The following allows Qt style sheets to work on a derived class (#derivedClassName)
    QStyleOption options;
    options.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
   
}
