#include "exceptions.hpp"

ResponseEx::ResponseEx(std::string message) : message(message){}

std::string ResponseEx::String(){
    return message;
}
