#include <string>
#include <unordered_map>
#include "include/utils.hpp"


MOS getMOSTypeByString(std::string str){
    switch (str[0]){
    case 'n':
        return MOS::N;
    case 'p':
        return MOS::P;
    default:
        throw std::runtime_error("Invalid MOS Type.");
    }
}

double getNumberByString(std::string str){
    return std::stod(str.substr(2, str.size() - 3));
}

