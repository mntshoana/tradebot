
#ifndef TEXT_PANEL_HEADER
#define TEXT_PANEL_HEADER

#include <QTextEdit>

class TextPanel : public QWidget {
    Q_OBJECT
    static QTextEdit* text;
    static bool isInitialized;
public:
    TextPanel& operator<< (std::string str);
    QTextEdit& getQText();
    void setGeometry(int ax, int ay, int aw, int ah);
    static void init(QWidget* parent);
};

#endif
