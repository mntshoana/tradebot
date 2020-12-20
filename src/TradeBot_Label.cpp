#include "TradeBot_Label.hpp"

Label::Label(std::string title, QWidget* parent) : QTextBrowser(parent) {
    setReadOnly(true);
    rename(title);
}

void Label::rename(std::string title) {
    std::stringstream ss;
    ss << std::fixed;
    ss << R"(
            <style>
            table {width: 100%;}
            tr { padding: 15px;}
            td, th {
                padding: 2px 4px 1px 2px;
                text-align: center;
            }
            tr th {
                color: rgb(173, 176, 182);
            }
            a {
                color: rgb(173, 176, 182);
                text-decoration: none;
            }
            .Chosen a {
                color: white;
                text-decoration: underline rgb(255, 241, 86) solid;
            }
            </style>
            <table width=100%>
                <tr>)";
    ss << title;
    ss << "</tr>";
    ss << "</table>\n";
    setHtml(ss.str().c_str());
}
