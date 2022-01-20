#ifndef LINE_BLOCK_HEADER
#define LINE_BLOCK_HEADER

#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>

class LineBlock : public QLineEdit
{
    QPushButton* button;
    Q_OBJECT
public:
    LineBlock(QWidget* parent = nullptr, QPushButton* button = nullptr): QLineEdit(parent), button(button) { }
    void keyPressEvent(QKeyEvent *event) override;
};

#endif /* line_block_header */
