#include <iostream>
#include <string>
#include "user.pb.h"

/*
一个远程服务
*/
class UserService : public fixbug::UserServiceRpc{//提供rpc服务
public:
    bool Login(std::string name, std::string pwd){
        std::cout << "doing local service: login" << std::endl;
        std::cout << "name" << name << "pwd" << pwd << std::endl;
        return true;
    }
    /*
    重写基类UserServiceRpc的虚函数，下面这些方法都是框架直接调用的
    1. caller ==> Login(LogRequest)  ==> muduo ==> callee
    2. callee ==> Login(LogRequest) ==> 交到下面重写的Login方法

    */
    virtual void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginReponse* response,
                       ::google::protobuf::Closure* done)
    {
        //框架给业务上报请求参数，业务获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();
        
        //做本地业务
        bool loginResult = Login(name, pwd);    
        
        //把响应写入,包括错误码、错误消息、返回值
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response -> set_success(loginResult);

        //执行回调操作, 执行响应对象的序列化和网络发送（由框架完成）
        done->Run();
    }
};