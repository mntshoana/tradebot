#ifndef WITHDRAW_PANEL_HPP
#define WITHDRAW_PANEL_HPP

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

#include "lunoclient.hpp"
#include "lineBlock.hpp"
#include "textPanel.hpp"

class PendingWithdrawals : public QWidget {
    std::vector<Luno::Withdrawal> userWithdrawals;
    TextPanel text;
public:
    QVBoxLayout *format;
    QHBoxLayout *line;
    
    PendingWithdrawals(QWidget* parent = nullptr);
    void loadItems();
    void reloadItems();
    void createItem (Luno::Withdrawal&);
    void createTitle ();
    void pushBack(Luno::Withdrawal& w);
    void paintEvent(QPaintEvent *);
};

class WithdrawPanel : public QWidget {
    Q_OBJECT
private:
    std::vector<Luno::Balance> userBalances;

    TextPanel text;
    
    QComboBox* assetBox;
    QLabel  *lblAmount,
            *lblBalance, *lblInstantWithdrawal,
            *lblPendingTitle;
    
    LineBlock *txtAmount;
    QCheckBox *cbxFastWithdraw;
    QGridLayout* panelLayout;
    QPushButton* withdraw;
    
    PendingWithdrawals *pending;
    
public:
    QGroupBox *boundingBox;
    
    static std::string floatToString(float val, const int decimals = 6);
    void loadItems ();
    void reloadItemsUsing(std::vector<Luno::Balance>& toCopy);
    WithdrawPanel(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent *);
    
    void lightTheme();
    void darkTheme();
};

#endif
