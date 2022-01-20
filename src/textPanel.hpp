
#ifndef TEXT_PANEL_HEADER
#define TEXT_PANEL_HEADER

#include <QTextEdit>
#include <QTextBrowser>

class TextPanel : public QWidget {
    Q_OBJECT
    static QTextEdit* text;
    static bool initialized;
    
    TextPanel (QWidget* parent);
    ~TextPanel ();
public:
    static TextPanel* textPanel;
    
    QTextEdit& getQText() const;
    void setGeometry(int ax, int ay, int aw, int ah);
    
    static void init(QWidget* parent);
    static void destroy();
    static bool isInitialized();
};

QTextEdit& operator << (QTextEdit& stream, const std::string& variable);
TextPanel& operator << (TextPanel& stream, const std::string& variable);
QTextBrowser& operator<< (QTextBrowser& stream, std::string& variable);
QTextEdit* operator << (QTextEdit* stream, const std::string& variable);
TextPanel* operator << (TextPanel* stream, const std::string& variable);
QTextBrowser* operator<< (QTextBrowser* stream, std::string& variable);

/* Type Helpers
 *
 *  Easily allow other objects to be printed through calling their toString() methods
 */
// Use only in headers
#define declarePrintableList(type) \
    TextPanel& operator << (TextPanel& stream, const std::vector<type>& vars); \
    QTextEdit& operator << (QTextEdit& stream, const std::vector<type>& vars); \
    TextPanel* operator << (TextPanel* stream, const std::vector<type>& vars); \
    QTextEdit* operator << (QTextEdit* stream, const std::vector<type>& vars)
#define declarePrintable(type) \
    TextPanel& operator << (TextPanel& stream, const type& variable); \
    QTextEdit& operator << (QTextEdit& stream, const type& variable); \
    TextPanel* operator << (TextPanel* stream, const type& variable); \
    QTextEdit* operator << (QTextEdit* stream, const type& variable)
// Use only in translation files (cpp)
#define printableDefinition(type) \
    TextPanel& operator << (TextPanel& stream,  const type& variable) { \
    stream.getQText().append(variable.toString().c_str()); \
        return stream; \
    }\
    QTextEdit& operator << (QTextEdit& stream, const type& variable) { \
    stream.append(variable.toString().c_str()); \
        return stream; \
    }\
    TextPanel* operator << (TextPanel* stream, const type& variable) { \
        (*stream) << variable; \
        return stream; \
    } \
    QTextEdit* operator << (QTextEdit* stream, const type& variable) { \
        (*stream) << variable; \
        return stream; \
    }

            
#define printableList(type) \
    TextPanel& operator << (TextPanel& stream,  const std::vector<type>& vars) { \
        for (const type& variable : vars) \
            stream << variable; \
        return stream; \
    }\
    QTextEdit& operator << (QTextEdit& stream, const std::vector<type>& vars) { \
        for (const type& variable : vars) \
            stream << variable; \
        return stream; \
    }\
    TextPanel* operator << (TextPanel* stream, const std::vector<type>& vars) { \
        (*stream) << vars; \
        return stream; \
    } \
    QTextEdit* operator << (QTextEdit* stream, const std::vector<type>& vars) { \
        (*stream) << vars; \
        return stream; \
    }
// end
// This could easily be simplified using abstract classes

#endif
