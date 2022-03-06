#pragma once
#include <unordered_map>
#include <string>

//rpcserverip rpcserverport zookeeperip zookeeperport
class MprpcConfig{
public:
    void LoadConfigFile(const char *config_file);
    std::string Load(const char *key);
private:
    std::unordered_map<std::string, std::string> m_configMap;
};