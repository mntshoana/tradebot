#include "requestManager.hpp"

requestManger::requestManger(){
    timer = new QTimer();
}

requestManger::~requestManger(){
    delete timer;
}
