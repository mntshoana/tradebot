#ifndef Job_Task_hpp
#define Job_Task_hpp

#include <functional>
#include "exceptions.hpp"
#include "textPanel.hpp"

class Task {
protected:
    std::function<void (void)> lamdaFunction;
    int recommendedWait;
    bool fastTrack;
    bool alwaysExecute = false;
public:
    bool repeat;
    int wait;
    
    Task( std::function<void (void)> lamdaFunction, bool fastTrack = false);
    void performJob();
    void updateWaitTime(int time);
    void setAsFast();
    void setAsSlow();
    void setToAlwaysExecute();
    bool isSetToAlwaysExecute();
    void setRepeat(bool status);
    bool isFast();
    void resetTimer();
    
};

#endif /* Job_Task_hpp */
