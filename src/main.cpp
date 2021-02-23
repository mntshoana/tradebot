#include "tradeBot.hpp"
#include <signal.h>

void sigtermClose(int ){
    QCoreApplication::quit();
}
int main(int argc, char* argv[]) {
    QApplication* entry = new QApplication(argc, argv);
    TradeBot* app = new TradeBot();
    signal(SIGTERM, [] (int)->void{QCoreApplication::quit();});
    app->show();
    return entry->exec();
}
