#include "textPanel.hpp"

bool TextPanel::isInitialized = false;
QTextEdit* TextPanel::text = nullptr;

TextPanel& TextPanel::operator<< (std::string str) {
    text->append(str.c_str());
    return *this;
}

void TextPanel::init(QWidget* parent){
    if (isInitialized)
        return;
    text = new QTextEdit(parent);
    text->document()->setDocumentMargin(10);
    text->setStyleSheet("QTextEdit { border:0;}");
    text->setText("");
    
    isInitialized = true;
}

QTextEdit& TextPanel::getQText(){
    return *text;
}

void TextPanel::setGeometry(int ax, int ay, int aw, int ah) {
    text->setGeometry(ax, ay, aw, ah);
}
