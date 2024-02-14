#ifndef exceptions_hpp
#define exceptions_hpp

#include <string>
#define errorLiner std::string("[Error] : At ") \
    + __FILE__ + ": line " + std::to_string(__LINE__) \
    + ". "
#define errorLinerWithMessage(msg) std::string("[Error] " #msg ": At")  \
    + __FILE__ + ": line " + std::to_string(__LINE__) \
    + ". "
class ResponseEx {
private:
    std::string message;
public:
    ResponseEx(std::string message);
    std::string String();
};
#endif /* exceptions_hpp */
