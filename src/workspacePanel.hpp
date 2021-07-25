#ifndef WORKSPACE_PANEL_HEADER
#define WORKSPACE_PANEL_HEADER

#include "pendingOrders.hpp"
#include "balancePanel.hpp"
#include "withdrawPanel.hpp"
#include "textPanel.hpp"

//
class WorkspacePanel : public QWidget {
    Q_OBJECT
    
public:
    QTabWidget *tabWidget;
    
    TextPanel text;
    PendingOrdersPanel *pendingOrders;
    BalancePanel *userBalances;
    WithdrawPanel *withdrawals;
    
    WorkspacePanel(QWidget* parent = nullptr);
    
    void setGeometry(int ax, int ay, int aw, int ah);
    void lightTheme();
    void darkTheme();
};

#endif
