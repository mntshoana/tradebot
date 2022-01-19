#ifndef TradeBot_hpp
#define TradeBot_hpp

#include <QApplication>
#include <QPalette>

#include <QtCore/QDateTime>

#include "objectivec.h"

#include "window.hpp"

#include "jobManager.hpp"

class TradeBot : public QWidget
{
    Q_OBJECT
public:
    TradeBot (QWidget *parent = 0);
private:
    void updateInterval(const std::string& period);
    
private:
    std::thread thread;
    
    HomeView* home;
    Window* current;
    
    JobManager manager;
    
    void loadTickData();
    void downloadTickData();
    void displayTickData();
    void updateTheme();
    void updatePanels();
    
    bool eventFilter(QObject *obj, QEvent *event)override;
    void enqueueJob(Task* job);
    
    public slots:
    void onEnqueueUserOrder(std::string orderID);
    
    private slots:
    void cleanup();

};
#endif /* TradeBot_hpp */
