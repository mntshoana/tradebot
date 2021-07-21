#include "orderPanel.hpp"

template<>
OrderPanel& operator<< <std::string>(OrderPanel& stream, std::string str)
{
    if (stream.orderViewIsEmpty)
    {
        stream.body->append(str.c_str()); // update content
        auto step = stream.body->verticalScrollBar()->singleStep();
        stream.body->verticalScrollBar()->setValue(step * 107.8);
        stream.orderViewIsEmpty = false;
    }
    else {
        auto y = stream.body->verticalScrollBar()->value();
        stream.body->setHtml(str.c_str()); // update content
        if (y != 0)
            stream.body->verticalScrollBar()->setValue(y);
        else{
            auto step = stream.body->verticalScrollBar()->singleStep();
            stream.body->verticalScrollBar()->setValue(step * 90);
        }
    }
    return stream;
}
