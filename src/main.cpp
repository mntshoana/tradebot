#include "tradeBot.hpp"

int main(int argc, char* argv[]) {
    QApplication* entry = new QApplication(argc, argv);
    TradeBot* app = new TradeBot();
    app->show();
    return entry->exec();
}
