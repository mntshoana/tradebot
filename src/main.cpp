#include "tradeBot.hpp"
#include <signal.h>
#include <iostream>
#include <fstream>
#include <objectivec.h>

void sigtermClose(int ){
    QCoreApplication::quit();
}
int main(int argc, char* argv[]) {
    try {
        QApplication* entry = new QApplication(argc, argv);
        TradeBot* app = new TradeBot();
        signal(SIGTERM, [] (int)->void{QCoreApplication::quit();});
        app->show();
        return entry->exec();
    } catch (const std::exception &exc) {
        // Create and open a text file
        std::ofstream MyFile(absolutePath() + "exception.txt");
        
        // Write to the file
        MyFile << errorLiner + exc.what();;
        
        // Close the file
        MyFile.close();
        
    }
    return -1;
}
