#include "mprpcconfig.h"
#include "util.h"

#include <iostream>
#include <string>

void MprpcConfig::LoadConfigFile(const char* config_file){
    FILE *pf = fopen(config_file, "r");
    if(pf == NULL){
        std::cout << config_file << "is not exist!" << std::endl;
    }
    //1. 注释 2. 正确的配置项 =  3.去掉开头多余的空行
    while(!feof(pf)){
        char buf[512];
        fgets(buf, 512, pf);

        //去掉字符串前面多余的空格
        std::string src_buf(buf);
        eraseFrontAndBackSpace(src_buf);
        if(src_buf.size() == 0 || src_buf[0] == '#'){
            continue;
        }
        //解析配置项
        std::string key;
        std::string value;
        if(parseConfig(src_buf, key, value, '=') == -1){
            continue;
        }
        m_configMap.insert({key, value});
    }
    for(auto it:m_configMap){
        std::cout << it.first << it.second << std::endl;
    }
}
std::string MprpcConfig::Load(const char *key){
    auto x = m_configMap[key];
    auto it = m_configMap.find(key);
    if(it != m_configMap.end()){
        return it -> second;
    }
    return "";
}