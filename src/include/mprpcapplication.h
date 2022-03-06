#pragma once
#include "mprpcconfig.h"

//mprpc框架初始化类
class MprpcApplication{
public:
    static void Init(int argc, char** argv);
    static MprpcApplication& GetIntance();
    static MprpcConfig& GetConfig();
private:
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
    static MprpcConfig config;
};