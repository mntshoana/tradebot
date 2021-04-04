#ifndef OPEN_ORDER_PANEL_HPP
#define OPEN_ORDER_PANEL_HPP
#include <QApplication>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QListWidget>

class OpenOrderPanel : public QWidget {
    Q_OBJECT
public:
    QListWidget * list;
    
    void CreateItem (QString& TheText);
    OpenOrderPanel(QWidget* parent = nullptr);
    
};

#endif
