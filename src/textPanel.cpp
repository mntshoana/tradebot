#include "textPanel.hpp"

QTextEdit& operator << (QTextEdit& stream, const std::string& variable){
    stream.append(variable.c_str());
    return stream;
}
TextPanel& operator << (TextPanel& stream, const std::string& variable){
    stream.getQText() << variable;
    return stream;
}
QTextBrowser& operator<< (QTextBrowser& stream, std::string& str){
    stream.append(str.c_str());
    return stream;
}

QTextEdit* operator << (QTextEdit* stream, const std::string& variable){
    (*stream) << variable;
    return stream;
}
TextPanel* operator << (TextPanel* stream, const std::string& variable){
    stream->getQText() << variable;
    return stream;
}
QTextBrowser* operator<< (QTextBrowser* stream, std::string& str){
    (*stream) << str;
    return stream;
}

bool TextPanel::initialized = false;
QTextEdit* TextPanel::text = nullptr;
TextPanel* TextPanel::textPanel = nullptr;

TextPanel::TextPanel (QWidget* parent) : QWidget(parent){
    text = new QTextEdit(this);
    text->setObjectName("TextEditPanel");
    text->document()->setDocumentMargin(10);
    text->setStyleSheet("QTextEdit { border:0;}");
    text->setText("");
}
TextPanel::~TextPanel (){
    if (text) {
        delete text;
        text = nullptr;
    }
}

void TextPanel::init(QWidget* parent){
    if (initialized)
        return;
    textPanel = new TextPanel(parent);
    initialized = true;
}

void TextPanel::destroy(){
    if (initialized && textPanel){
        delete TextPanel::textPanel;
        textPanel = nullptr;
        initialized = false;
    }
}

static bool isInitialized(){
    return isInitialized;
}

QTextEdit& TextPanel::getQText() const{
    return *text;
}

void TextPanel::setGeometry(int ax, int ay, int aw, int ah) {
    text->setGeometry(0, 0, aw, ah);
}
