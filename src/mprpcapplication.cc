#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig MprpcApplication::config;

void ShowArgHelp(){
    std::cout << "format: command -i configfile>" << std::endl;
}
void MprpcApplication::Init(int argc, char** argv){
    if (argc < 2) {
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while( (c = getopt(argc, argv, "i:")) != -1){
        switch (c)
        {
        case 'i': 
            config_file = optarg;
            break;
        case '?':
            ShowArgHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
       
    }
    
    //开始加载配置文件 rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
    config.LoadConfigFile(config_file.c_str());
}
MprpcApplication& MprpcApplication::GetIntance(){
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig(){
    return config;
}