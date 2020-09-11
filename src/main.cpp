#include <QApplication>
#include <QTextEdit>
#include "client.hpp"

QTextEdit& operator<< (QTextEdit& stream, std::string str)
{
    stream.append(str.c_str());
    return stream;
}


int main(int argc, char* argv[]) {
    QApplication entry(argc, argv);
    QWidget app;
    QTextEdit text(&app);
    text.resize(1080, 720);
    text.setText("");
    app.show();
    
    Client client;
    try {
        //OrderBook ob = client.getOrderBook("XBTZAR");
        //Tick tick = client.getTick("XBTZAR");
        //std::vector<Tick> ticks = client.getTicks();
        //std::vector<Trade> trades = client.getTrades("XBTZAR",1599102072141);
        //text << ob << "\n" << tick << "\n" << ticks << trades;
        
        Fee fee = client.getFeeInfo("XBTZAR");
        std::vector<UserOrder> orders = client.getUserOrders("","", 20);
        // beta function
        std::vector<UserTrade> trades = client.getUserTrades("XBTZAR");
        // end beta
        text << fee << "\n" << orders << "\n" << trades;
    }
    catch (ResponseEx ex){
        text << ex.String();
    }
    return entry.exec();
}
