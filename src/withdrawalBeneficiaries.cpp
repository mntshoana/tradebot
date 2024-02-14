#include "withdrawalBeneficiaries.hpp"

struct SepFacet : std::numpunct<char> {
   /* use space as separator */
   char do_thousands_sep() const { return ' '; }

   /* digits are grouped by 3 digits each */
   std::string do_grouping() const { return "\3"; }
};

std::string WithdrawalBeneficiaryPanel::floatToString(float val, const int decimals )
{
    std::ostringstream out;
    out.imbue(std::locale(std::locale(), new SepFacet));
    out.precision(decimals);
    out << std::fixed << val;
    return out.str();
}

WithdrawalBeneficiaryPanel::WithdrawalBeneficiaryPanel(float amount, bool isFastWithdrawal, std::vector<Luno::Beneficiary>& beneficiaries,
    std::vector<Luno::Withdrawal>& userWithdrawals) : QWidget(nullptr), amount(amount),
                                                    isFastWithdrawal(isFastWithdrawal),
                                                    beneficiaries(beneficiaries), userWithdrawals(userWithdrawals){
    //setAutoFillBackground(true);
    setObjectName("WithdrawalBeneficiaries");
    
    TextPanel::init(nullptr);
    text = TextPanel::textPanel;
    
    format = new QVBoxLayout;
    format->setSpacing(0);
    format->setAlignment(Qt::AlignTop);
    setLayout(format);
    
    createTitle();
    loadItems();
}

void WithdrawalBeneficiaryPanel::createItem (Luno::Beneficiary& beneficiary){
    setMinimumWidth(700);
    
    std::string accountNumber;
    std::string accountType;
    std::string country;
    std::string accountHolder;
    bool supportsFastWithdrawal;
    QLabel *lblBank = new QLabel(QString::fromStdString(beneficiary.bank));
    QLabel *lblAccNr = new QLabel(QString::fromStdString(beneficiary.accountNumber));
    QLabel *lblCountry = new QLabel(QString::fromStdString(beneficiary.country));
   
    QPushButton *but = new QPushButton ( "Withdraw" );
    
    lblBank->setMinimumHeight(15);lblBank->setFixedHeight(15);lblBank->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblAccNr->setFixedHeight(15);lblAccNr->setFixedHeight(15);lblAccNr->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblCountry->setFixedHeight(15);lblCountry->setFixedHeight(15);lblCountry->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    // add the label and button to the layout
    line = new QHBoxLayout;
    line->addWidget( lblBank,9, Qt::AlignLeft);
    line->addWidget( lblAccNr,7, Qt::AlignLeft);
    line->addWidget( lblCountry,6, Qt::AlignLeft);
    line->addWidget( but,2, Qt::AlignCenter );
    line->setContentsMargins(1,0,0,0);
    line->setAlignment(Qt::AlignTop);
    
    line->addStrut(10);
    
    format->addLayout(line);

    connect(but, &QPushButton::clicked, this, [this, beneficiary] () {
        withdraw(beneficiary.id);
            
    });
}

void WithdrawalBeneficiaryPanel::createTitle (){
    QLabel *lblBank = new QLabel ( QString::fromStdString("Bank" ) );
    QLabel *lblAccountNr = new QLabel ( QString::fromStdString("Account Number" ) );
    QLabel *lblCountry = new QLabel ( QString::fromStdString("Country") );
    
    lblBank->setFixedHeight(15);lblBank->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblAccountNr->setFixedHeight(15);lblAccountNr->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lblCountry->setFixedHeight(15);lblCountry->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    
    line = new QHBoxLayout;
    line->addWidget( lblBank,9, Qt::AlignLeft);
    line->addWidget( lblAccountNr,7, Qt::AlignLeft);
    line->addWidget( lblCountry,6, Qt::AlignLeft);
    line->addWidget( new QLabel(""),2, Qt::AlignCenter);
    line->setContentsMargins(1,0,0,0);
    line->setAlignment(Qt::AlignTop);
    format->addLayout(line);
}

void WithdrawalBeneficiaryPanel::loadItems (){
    for (Luno::Beneficiary& b : beneficiaries)
        createItem( b );
}

void WithdrawalBeneficiaryPanel::paintEvent(QPaintEvent *){
    // The following allows Qt style sheets to work on a derived class (#derivedClassName)
    QStyleOption options;
    options.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &options, &painter, this);
   
}

void WithdrawalBeneficiaryPanel::withdraw(std::string beneficiaryID){
    try {
        Luno::Withdrawal w = Luno::LunoClient::withdraw(amount, isFastWithdrawal, beneficiaryID);
        userWithdrawals.push_back(w);
        
        text->getQText() << w;
    }
    catch (ResponseEx ex){
        *text << errorLinerWithMessage("Unable to withdraw!") + ex.String();
    } catch (std::invalid_argument ex) {
        *text << errorLiner + ex.what();
    }
}
