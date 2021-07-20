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

    withdraw = new QPushButton(this);
    
    
    txtAmount = new LineBlock(this, withdraw);
    lblBalance = new QLabel("", this);
    
    withdrawLayout = new QGridLayout;
    boundingBox = new QGroupBox(this);
    withdrawLayout->addWidget(lblAsset, 1, 1);
    withdrawLayout->addWidget(assetBox, 1, 2);
    withdrawLayout->addWidget(lblAmount, 2, 1);
    withdrawLayout->addWidget(txtAmount, 2, 2);
    withdrawLayout->addWidget(lblBalance, 2, 3);
    withdrawLayout->addWidget(withdraw, 3, 1, 1, 2,
                              Qt::AlignmentFlag::AlignCenter);
    
    //    std::string period = assetBox->currentText().toStdString();

    boundingBox->setLayout(withdrawLayout);
    
    connect(assetBox, &QComboBox::currentTextChanged, this,
            [this](const QString &selection){
                for (Luno::Balance bal : userBalances){
                    if (bal.asset == selection.toStdString()){
                        bool isZar = (bal.asset == "ZAR");
                        std::string label =floatToString(bal.balance, ( isZar ? 2 : 6 ));
                        lblBalance->setText(QString::fromStdString(label));
                    }
                }
    });
    connect(withdraw,
            &QPushButton::clicked, this,[this](){
        text << "Withdraw button clicked. To Widthraw " + txtAmount->text().toStdString();
        try {
        std::string result = Luno::LunoClient::withdraw(atoi(txtAmount->text().toStdString().c_str()), true);
            text << result;
        }
        catch (ResponseEx ex){
            text << " [Error] Unable to withdraw! At " + std::string(__FILE__) + ", line: " + std::to_string(__LINE__);
            text << ex.String();
        }
    });
}

std::string WithdrawPanel::floatToString(float val, const int decimals )
{
    std::ostringstream out;
    out.precision(decimals);
    out << std::fixed << val;
    return out.str();
}


void WithdrawPanel::loadItems (){
    try{
        userBalances = Luno::LunoClient::getBalances();
        
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
