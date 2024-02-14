#ifndef WITHDRAWAL_BENEFICIARIES_HPP
#define WITHDRAWAL_BENEFICIARIES_HPP

#include <QDateTime>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPainter>
#include <QPaintEvent>

#include <QGroupBox>
#include <QLayout>
#include <QStyleOption>

#include "client/lunoclient/lunoclient.hpp"
#include "client/valrclient/valrclient.hpp"
#include "lineBlock.hpp"
#include "textPanel.hpp"

class WithdrawalBeneficiaryPanel : public QWidget {
    std::vector<Luno::Withdrawal> userWithdrawals;
    std::vector<Luno::Beneficiary>& beneficiaries;
    TextPanel* text;
    
    float amount;
    bool isFastWithdrawal;
public:
    QVBoxLayout *format;
    QHBoxLayout *line;
    
    
    static std::string floatToString(float val, const int decimals = 6);

    WithdrawalBeneficiaryPanel(float, bool, std::vector<Luno::Beneficiary>&, std::vector<Luno::Withdrawal>&);
    void loadItems();
    void createItem (Luno::Beneficiary&);
    void createTitle ();
    void pushBack(Luno::Beneficiary& w);
    void paintEvent(QPaintEvent *);
    void withdraw(std::string beneficiaryID);
};

#endif
