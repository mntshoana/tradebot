#ifndef exceptions_hpp
#define exceptions_hpp

#include <string>

class ResponseEx {
private:
    std::string message;
public:
    ResponseEx(std::string message);
    std::string String();
};
#endif /* exceptions_hpp */
