#include "openOrderPanel.hpp"

OpenOrderPanel::OpenOrderPanel(QWidget* parent) : QWidget(parent){
    list = new QListWidget(this);
    list->setGeometry(0, 500, 1180, 220);
    QFile file ( "/Users/macgod/Dev/tradebot/tradebot/src/mylist.txt" );
    if ( !file.open ( QIODevice::ReadOnly ) ) {
        QMessageBox::information ( 0, "error", file.errorString() );
    }

    QTextStream in ( &file );
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        CreateItem ( line );
    }

    file.close();
}
void OpenOrderPanel::CreateItem (QString& TheText  )
{

    QListWidgetItem* item = new QListWidgetItem ( "" );
    list->addItem ( item );
    // add a place hodler for label and button

    setLayout ( new QHBoxLayout() );
    QPushButton *but = new QPushButton ( "Do it" );
    QLabel *lab = new QLabel ( TheText );
    // make row a bit bigger
    item->setSizeHint ( QSize ( item->sizeHint().width(), 30 ) );
    // add the label and button to the layout
    layout()->addWidget ( lab );
    layout()->addWidget ( but );
    // reduce the space around it abit
    layout()->setContentsMargins ( 1, 1, 1, 1 );
    // set this combined widget for the row
    //list->setItemWidget ( item, this );
}
