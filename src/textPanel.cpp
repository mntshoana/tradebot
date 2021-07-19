#include "textPanel.hpp"

bool TextPanel::isInitialized = false;
QTextEdit* TextPanel::text = nullptr;

TextPanel& TextPanel::operator<< (std::string str) {
    text->append(str.c_str());
    return *this;
}

void TextPanel::init(QWidget* parent){
    TextPanel::text = new QTextEdit(parent);
    parent->setGeometry(0, 515, 930, 205);
    parent->setStyleSheet("QTextEdit { padding-left:5; padding-top:10;}");
    text->setGeometry(0, 0, 930, 205);
    text->setText("");
    
    isInitialized = true;
}

TextPanel::TextPanel(QWidget* parent) : QWidget(parent) {
    if (!isInitialized)
        TextPanel::init(this);
}
