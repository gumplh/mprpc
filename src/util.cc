#include "util.h"

void eraseFrontAndBackSpace(std::string &str){
    if(str.size() == 0){
        return;
    }
    int idxFront = str.find_first_not_of(' ');
    int idxBack = str.find_last_not_of(' ');
    idxFront = idxFront == -1 ? 0 : idxFront;
    idxBack = idxBack == -1 ? str.size() - 1 : idxBack;
    str = str.substr(idxFront, idxBack - idxFront + 1);
}

int parseConfig(const std::string &src_buf, std::string &key, std::string &value, const char &splitChar){
    int idx = src_buf.find(splitChar);
    if(idx == -1){
        return -1;
    }
    key = src_buf.substr(0, idx);
    value = src_buf.substr(idx + 1, src_buf.size() - idx - 1);
    eraseLineFeed(key);
    eraseFrontAndBackSpace(key);
    eraseLineFeed(value);
    eraseFrontAndBackSpace(value);
    
    return 1;
}

void eraseLineFeed(std::string &str){
    int idx = str.find('\n');
    if(idx != -1){
        str.erase(str.begin() + idx, str.end());
    }

}