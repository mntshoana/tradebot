
#ifndef TEXT_PANEL_HEADER
#define TEXT_PANEL_HEADER

#include <QTextEdit>

class TextPanel : public QWidget {
    Q_OBJECT
    static QTextEdit* text;
    static bool isInitialized;
public:
    TextPanel(QWidget* parent = nullptr);
    TextPanel& operator<< (std::string str);
    void init(QWidget* parent);
};

#endif
