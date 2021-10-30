
#ifndef TEXT_PANEL_HEADER
#define TEXT_PANEL_HEADER

#include <QTextEdit>
#include <QTextBrowser>

class TextPanel : public QWidget {
    Q_OBJECT
    static QTextEdit* text;
    static bool isInitialized;
    
    TextPanel (QWidget* parent);
public:
    static TextPanel* textPanel;

    TextPanel& operator<< (const std::string str);
    
    QTextEdit& getQText() const;
    void setGeometry(int ax, int ay, int aw, int ah);
    
    static void init(QWidget* parent);
};

QTextEdit& operator << (QTextEdit& stream, std::string& variable);
TextPanel& operator << (TextPanel& stream, std::string& variable);
QTextBrowser& operator<< (QTextBrowser& stream, std::string& variable);
QTextEdit* operator << (QTextEdit* stream, std::string& variable);
TextPanel* operator << (TextPanel* stream, std::string& variable);
QTextBrowser* operator<< (QTextBrowser* stream, std::string& variable);

#endif
