#ifndef DROP_DOWN_BOX_HEADER
#define DROP_DOWN_BOX_HEADER

#include <QComboBox>
#include "textPanel.hpp"

class DropDownBox : public QComboBox
{
    Q_OBJECT
public:
    DropDownBox(QWidget* parent = nullptr) ;
};

#endif /* drop_down_box_header */
