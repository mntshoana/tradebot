
#ifndef TEXT_PANEL_HEADER
#define TEXT_PANEL_HEADER

#include <QTextEdit>

class TextPanel : public QWidget {
    Q_OBJECT
    static QTextEdit* text;
    static bool isInitialized;
    
    TextPanel (QWidget* parent);
public:
    static TextPanel* textPanel;

    TextPanel& operator<< (const std::string str);
    
    QTextEdit& getQText();
    void setGeometry(int ax, int ay, int aw, int ah);
    operator QTextEdit&();
    
    static void init(QWidget* parent);
};

#endif
