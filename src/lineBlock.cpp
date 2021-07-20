#include "lineBlock.hpp"

void LineBlock::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Return && button != nullptr)
    {
        emit button->clicked();
    }
    else
    {
        QLineEdit::keyPressEvent(event);
    }
}
