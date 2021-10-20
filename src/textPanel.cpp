#include "textPanel.hpp"

bool TextPanel::isInitialized = false;
QTextEdit* TextPanel::text = nullptr;
TextPanel* TextPanel::textPanel = nullptr;

TextPanel::TextPanel (QWidget* parent) : QWidget(parent){
    text = new QTextEdit(this);
    text->setObjectName("TextEditPanel");
    text->document()->setDocumentMargin(10);
    text->setStyleSheet("QTextEdit { border:0;}");
    text->setText("");
}

TextPanel& TextPanel::operator<< (std::string str) {
    text->append(str.c_str());
    return *this;
}

void TextPanel::init(QWidget* parent){
    if (isInitialized)
        return;
    textPanel = new TextPanel(parent);
    isInitialized = true;
}

QTextEdit& TextPanel::getQText(){
    return *text;
}

void TextPanel::setGeometry(int ax, int ay, int aw, int ah) {
    text->setGeometry(0, 0, aw, ah);
}
