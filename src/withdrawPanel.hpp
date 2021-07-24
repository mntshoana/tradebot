#ifndef WITHDRAW_PANEL_HPP
#define WITHDRAW_PANEL_HPP

#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPainter>
#include <QPaintEvent>

#include <QGroupBox>
#include <QLayout>
#include <QStyleOption>

#include "lunoclient.hpp"
#include "lineBlock.hpp"
#include "textPanel.hpp"

class WithdrawPanel : public QWidget {
    Q_OBJECT
private:
    std::vector<Luno::Balance> userBalances;
    std::vector<Luno::Withdrawal> userWithdrawals;
    TextPanel text;
    
    QComboBox* assetBox;
    QLabel *lblAsset, *lblAmount,
            *lblBalance, *lblInstantWithdrawal;
    
    LineBlock *txtAmount;
    QCheckBox *cbxFastWithdraw;
    QGridLayout* panelLayout;
    QPushButton* withdraw;
    std::string floatToString(float val, const int decimals = 6);
public:
    QGroupBox *boundingBox;
    
    void loadItems ();
    void loadItems(std::vector<Luno::Balance>& toCopy);
    WithdrawPanel(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent *);
};

#endif
