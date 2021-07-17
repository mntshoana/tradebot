
#ifndef TEXT_PANEL_HEADER
#define TEXT_PANEL_HEADER

#include <QTextEdit>

class TextPanel : public QWidget {
    Q_OBJECT
    static QTextEdit* text;
    static bool isInitialized;
public:
    TextPanel& operator<< (std::string str);
    void init(QWidget* parent);
    TextPanel(QWidget* parent = nullptr);
    
};

#endif
