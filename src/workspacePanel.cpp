#include "workspacePanel.hpp"

WorkspacePanel::WorkspacePanel(QWidget* parent) {
    
    pendingOrders = new PendingOrdersPanel(this);
    pendingOrders->setObjectName("PendingOrders");
    
    userBalances = new BalancePanel(this);
    userBalances->setObjectName("UserBalances");
    
    withdrawals = new WithdrawPanel(this);
    
    autoPlayground = new AutoPlaygroundPanel(this);
    
    tabWidget = new QTabWidget(parent);
    tabWidget->addTab(&text, tr("Output"));
    tabWidget->addTab(pendingOrders, tr("Open Orders"));
    tabWidget->addTab(userBalances, tr("User Balances"));
    tabWidget->addTab(withdrawals, tr("Withdrawals"));
    tabWidget->addTab(autoPlayground, tr("Auto Playground"));
}

void WorkspacePanel::setGeometry(int ax, int ay, int aw, int ah) {
    tabWidget->setGeometry(ax, ay, aw, ah);
    text.setGeometry(ax-4, ay + 50, aw+4, ah - 50);
    autoPlayground->setGeometry(ax, ay, aw, ah);
}

void WorkspacePanel::lightTheme(){
    withdrawals->lightTheme();
    
    tabWidget->setStyleSheet(R"(WorkspacePanel  {
                                border-right-color: rgb(225, 225, 225);
                             } QTabWidget::tab-bar  {
                                left: 5px;
                                top: 28px;
                             } QTabWidget::pane {
                                border-top: 25px solid #fdfdfd;
                             } QTabBar::tab {
                                border-style: outset;
                                border-radius: 10px;
                                padding-left: 8px;
                                padding-right: 8px;
                                padding-top: 2px;
                                padding-bottom: 2px;
                                margin-bottom: 2px;
                              } QTabBar::tab:!first:!last {
                                border-radius: 0px;
                              } QTabBar::tab:first {
                                border-top-right-radius: 0px;
                                border-bottom-right-radius: 0px;
                              } QTabBar::tab:last {
                                border-top-left-radius: 0px;
                                border-bottom-left-radius: 0px;
                              } QTabBar::tab:selected {
                                color: rgb(255 , 255, 255);
                                border-top: 1px solid rgb(41 ,155, 255);
                                background-color: rgb(21 ,133, 255);
                                border-bottom: 1px solid rgb(1 ,123, 255);
                                margin-top: 2px;
                                margin-bottom: 0px;
                              } QTabBar::tab:!selected {
                                color: rgb(69 , 69, 69);
                                border-top: 1px solid rgb(230 ,230, 230);
                                background-color: rgb(255 ,255, 255);
                                border-bottom: 1px solid rgb(209, 209, 209);
                              } QTabBar::tab:!last:!selected {
                                 border-right: 1px solid rgb(209, 209, 209);
                              } QTextEdit, #PendingOrders, #UserBalances, #WithdrawalPanel {
                                background-color: rgb(253, 253, 253);
                                border-style: outset;
                                border-width: 0.5px;
                                border-top-color: rgb(165, 165, 165);
                                border-bottom-color: rgb(245, 245, 245);
                                border-left-color: rgb(185, 185, 185);
                                border-right-color: rgb(225, 225, 225);
                             } )");
}
void WorkspacePanel::darkTheme(){
    withdrawals->darkTheme();
    
    tabWidget->setStyleSheet(R"(WorkspacePanel  {
                                border-right-color: rgb(37, 37, 37);
                             } QTabWidget::tab-bar  {
                                left: 5px;
                                top: 28px;
                             } QTabWidget::pane {
                                border-top: 25px solid #1a1a1a;
                             } QTabBar::tab {
                                border-style: outset;
                                border-radius: 10px;
                                padding-left: 8px;
                                padding-right: 8px;
                                padding-top: 2px;
                                padding-bottom: 2px;
                                margin-bottom: 2px;
                             } QTabBar::tab:!first:!last {
                                border-radius: 0px;
                             } QTabBar::tab:first {
                                border-top-right-radius: 0px;
                                border-bottom-right-radius: 0px;
                             } QTabBar::tab:last {
                                border-top-left-radius: 0px;
                                border-bottom-left-radius: 0px;
                             } QTabBar::tab:selected {
                                color: rgb(241 , 231, 242);
                                border-top: 1px solid rgb(26 ,120, 220);
                                background-color: rgb(21 ,111, 215);
                                border-bottom: 1px solid rgb(10 ,55, 107);
                                margin-top: 2px;
                                margin-bottom: 0px;
                             } QTabBar::tab:!selected {
                                color: rgb(241 , 231, 242);
                                border-top: 1px solid rgb(87 ,87, 87);
                                background-color: rgb(83 ,83, 83);
                                border-bottom: 1px solid rgb(77 ,77, 77);
                             } QTextEdit, #PendingOrders, #UserBalances, #WithdrawalPanel {
                                background-color: rgb(28, 28, 28);
                                border-style: outset;
                                border-width: 0.5px;
                                border-top-color: rgb(32, 32, 32);
                                border-bottom-color: rgb(25, 25, 25);
                                border-left-color: rgb(28, 28, 28);
                                border-right-color: rgb(37, 37, 37);
                             } )");
}
