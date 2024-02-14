#include "withdrawPanel.hpp"

WithdrawPanel::WithdrawPanel(QWidget* parent) : QWidget(parent) {    
    setObjectName("WithdrawalPanel");
    
    TextPanel::init(parent);
    text = TextPanel::textPanel;
    
    beneficiaryPanel = nullptr;
    
    loadItems();
    
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
    
    lblInstantWithdrawal = new QLabel("Is instant?", this);
    cbxFastWithdraw = new QCheckBox(this);

    
    lblPendingTitle = new QLabel("List of pending withdrawals", this);
    pending = new PendingWithdrawals(this);
    
    
    panelLayout = new QGridLayout;
    boundingBox = new QGroupBox(this);
    panelLayout->addWidget(assetBox, 1, 2, 1, 2);
    panelLayout->addWidget(lblAmount, 2, 1);
    panelLayout->addWidget(txtAmount, 2, 2, 1, 2);
    panelLayout->addWidget(lblBalance, 2, 4, 1, 2);
    panelLayout->addWidget(lblInstantWithdrawal, 3, 1);
    panelLayout->addWidget(cbxFastWithdraw, 3, 2);
    panelLayout->addWidget(withdraw, 4, 2);
    panelLayout->addWidget(lblPendingTitle, 0, 6, 1, 5, Qt::AlignCenter);
    panelLayout->addWidget(pending, 1, 6, 4, 5);
    panelLayout->setContentsMargins(30, 30, 30, 60);

    boundingBox->setLayout(panelLayout);
    boundingBox->setGeometry(0, 1, 930, 220);
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
        *text << "Withdraw button clicked. To Widthraw " + txtAmount->text().toStdString();
        
        bool isFastWithdrawl = cbxFastWithdraw->isChecked();
        float amount = atof(txtAmount->text().toStdString().c_str()); // ZAR
        std::vector<Luno::Beneficiary> beneficiaries;
        
        try {
            beneficiaries = Luno::LunoClient::listBeneficiaries();
        } catch (ResponseEx ex){
            *TextPanel::textPanel << errorLiner + ex.String().c_str();
            return;
        } catch (std::invalid_argument ex) {
            *text << errorLiner + ex.what();
        }
        
        if (beneficiaries.size() == 1){
            try {
                Luno::Withdrawal w = Luno::LunoClient::withdraw(amount, isFastWithdrawl);
                pending->pushBack(w);
                
                text->getQText() << w;
            }
            catch (ResponseEx ex){
                *text << errorLinerWithMessage("Unable to withdraw!") + ex.String();
            } catch (std::invalid_argument ex) {
                *text << errorLiner + ex.what();
            }
        }
        else {
            if (beneficiaryPanel == nullptr){
                beneficiaryPanel = new WithdrawalBeneficiaryPanel(amount, isFastWithdrawl, beneficiaries, pending->userWithdrawals);
                    beneficiaryPanel->show();
            } else {
                beneficiaryPanel->close();
                beneficiaryPanel = new WithdrawalBeneficiaryPanel(amount, isFastWithdrawl, beneficiaries,
                                                                  pending->userWithdrawals);
                beneficiaryPanel->show();
            }
            
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
            *text << errorLiner +  ex.String().c_str();
    } catch (std::invalid_argument ex) {
        *text << errorLiner + ex.what();
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
    
void WithdrawPanel::lightTheme(){
    
    boundingBox->setStyleSheet(R"(QGroupBox {
                                    background-color: white;
                                    color: black;
                                    border: none;
                                 } QGroupBox::title {
                                    background-color:transparent;
                                 })");
    pending->setStyleSheet(R"(#PendingWithdrawals {
                               border-top: 1px solid #d4d4d4;
                               background-color: #fdfdfd;
                               border-bottom: 1px solid #cfcfcf;
                               color: black;
                               border: none;
                           })");
}

void WithdrawPanel::darkTheme(){
    boundingBox->setStyleSheet(R"(QGroupBox {
                                    background-color: #1e1e1e;
                                    color: white;
                                    border: none;
                                 } QGroupBox::title {
                                    background-color:transparent;
                                 })");
    pending->setStyleSheet(R"(#PendingWithdrawals {
                               border-top: 1px solid #292929;
                               background-color: #242424;
                               border-bottom: 1px solid #202020;
                               color: white;
                               border: none;
                           })");
}

void PendingWithdrawals::reloadItems(){
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

PendingWithdrawals::PendingWithdrawals(QWidget* parent) : QWidget(parent){
    //setAutoFillBackground(true);
    setObjectName("PendingWithdrawals");
    
    TextPanel::init(parent);
    text = TextPanel::textPanel;
    
    format = new QVBoxLayout;
    format->setSpacing(0);
    format->setAlignment(Qt::AlignTop);
    setLayout(format);
    
    createTitle();
    loadItems();
}

void PendingWithdrawals::createItem (Luno::Withdrawal& withdrawal){
    QLabel *lblDate = new QLabel ( QDateTime::fromMSecsSinceEpoch(withdrawal.createdTime).toString("ddd d, hh:mm") );
    QLabel *lblType = new QLabel ( QString::fromStdString(withdrawal.currency ) );
    
    std::string amount = WithdrawPanel::floatToString(withdrawal.amount, (withdrawal.currency == "ZAR" ? 2 : 6));
    QLabel *lblAmount = new QLabel ( QString::fromStdString(amount ) );
    std::string fee = WithdrawPanel::floatToString(withdrawal.fee, (withdrawal.currency == "ZAR" ? 2 : 6));
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
            *text << "Cancelled order!";
            *text << ("COMPLETE: " + result).c_str();
        }
        catch (ResponseEx& ex){
            *text << errorLinerWithMessage("Unable to cancel withdrawal!") + ex.String();
        } catch (std::invalid_argument ex) {
            *text << errorLiner + ex.what();
        }
    });
}

void PendingWithdrawals::createTitle (){
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

void PendingWithdrawals::loadItems (){
    try {
        std::vector<Luno::Withdrawal> retrieved = Luno::LunoClient::getWithdrawalList();
        for (Luno::Withdrawal& w : retrieved){
            if (w.status == "PENDING"){
                pushBack(w);
            }
        }
    } catch (ResponseEx ex){
            *text << errorLiner + ex.String().c_str();
    } catch (std::invalid_argument ex) {
        *text << errorLiner + ex.what();
    }
}
void PendingWithdrawals::paintEvent(QPaintEvent *){
    // The following allows Qt style sheets to work on a derived class (#derivedClassName)
    QStyleOption options;
    options.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
   
}

void PendingWithdrawals::pushBack(Luno::Withdrawal& w){
    if (w.status == "PENDING"){
        createItem( w );
        userWithdrawals.push_back(w);
    }
}
