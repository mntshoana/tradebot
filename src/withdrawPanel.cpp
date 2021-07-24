#include "withdrawPanel.hpp"

WithdrawPanel::WithdrawPanel(QWidget* parent) : QWidget(parent) {
    setGeometry(0, 500, 930, 220);
    
    loadItems();
    text.getQText() << userWithdrawals; // quick test of function only
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
    
    panelLayout->setContentsMargins(60, 30, 0, 0);

    boundingBox->setLayout(panelLayout);
    
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
        userWithdrawals = Luno::LunoClient::getWithdrawalList();
        
    } catch (ResponseEx ex){
            text << ex.String().c_str(); // To do:: should be an error stream here
    }
}

void WithdrawPanel::loadItems(std::vector<Luno::Balance>& toCopy){
    userBalances = toCopy;
}
void WithdrawPanel::paintEvent(QPaintEvent *) {
     // The following allows Qt style sheets to work on a derived class (#derivedClassName)
     QStyleOption options;
     options.initFrom(this);
     QPainter painter(this);
     style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
    
}
