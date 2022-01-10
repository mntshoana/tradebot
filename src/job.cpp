#include "job.hpp"

Task::Task( std::function<void (void)> lamdaFunction, bool fastTrack ){
    updateWaitTime(1);
    this->fastTrack = fastTrack;
    this->lamdaFunction = lamdaFunction;
    this->repeat = false;
}

void Task::updateWaitTime(int time) {
    wait = recommendedWait = time;
}

void Task::performJob() {
    try{
        lamdaFunction();
    } catch (ResponseEx ex){
        TextPanel::textPanel << ex.String();
    }
}

void Task::setAsFast() {
    fastTrack = true;
}
void Task::setAsSlow() { 
    fastTrack = false;
}
void Task::setToAlwaysExecute() {
    fastTrack = true;
    alwaysExecute = true;
}
bool Task::isSetToAlwaysExecute() {
    return alwaysExecute;
}
bool Task::isFast() {
    return fastTrack;
}
void Task::setRepeat(bool status){
    repeat = status;
}
void Task::resetTimer() {
    wait = recommendedWait;
}
