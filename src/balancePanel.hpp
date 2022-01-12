#ifndef USER_BALANCE_PANEL_HPP
#define USER_BALANCE_PANEL_HPP
#include <QFormLayout>
#include <QLabel>

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

#include "client/lunoclient/lunoclient.hpp"
#include "client/valrclient/valrclient.hpp"
#include "textPanel.hpp"

class BalancePanel : public QWidget {
    Q_OBJECT
private:
    std::vector<std::string> accountIDs;
    TextPanel* text;
    
    std::string floatToString(float val, const int decimals = 6);
public:
    QVBoxLayout *format;
    QHBoxLayout *line;
    std::vector<Luno::Balance> userBalances;
    
    void reloadItems();
    void createItem (Luno::Balance&);
    void createTitle ();
    void loadItems ();
    BalancePanel(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent *);
};

#endif
