#include "textPanel.hpp"

QTextEdit& operator << (QTextEdit& stream, std::string& variable){
    stream.append(variable.c_str());
    return stream;
}
TextPanel& operator << (TextPanel& stream, std::string& variable){
    stream.getQText() << variable;
    return stream;
}
QTextBrowser& operator<< (QTextBrowser& stream, std::string& str){
    stream.append(str.c_str());
    return stream;
}

QTextEdit* operator << (QTextEdit* stream, std::string& variable){
    (*stream) << variable;
    return stream;
}
TextPanel* operator << (TextPanel* stream, std::string& variable){
    stream->getQText() << variable;
    return stream;
}
QTextBrowser* operator<< (QTextBrowser* stream, std::string& str){
    (*stream) << str;
    return stream;
}

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

TextPanel& TextPanel::operator<< (const std::string str) {
    text->append(str.c_str());
    return *this;
}

void TextPanel::init(QWidget* parent){
    if (isInitialized)
        return;
    textPanel = new TextPanel(parent);
    isInitialized = true;
}

QTextEdit& TextPanel::getQText() const{
    return *text;
}

void TextPanel::setGeometry(int ax, int ay, int aw, int ah) {
    text->setGeometry(0, 0, aw, ah);
}
