#pragma once

#include "google/protobuf/service.h"
#include "mprpcapplication.h"
#include <string>
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Callbacks.h>
#include <muduo/base/Timestamp.h>
//框架提供的专门服务发布rpc服务的网络对象类
class RpcProvider{
public:
    // 框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);
    //启动rpc服务节点，提供rpc远程调用服务
    void Run();

private:
    //service信息
    struct ServiceInfo{
        google::protobuf::Service *m_service;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    //存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_ServiveInfoMap;
    //组合EventLoop
    muduo::net::EventLoop m_eventLoop;

    //新的socket连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr &conn);

    //已建立连接用户的读写时间回调
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

    //给closure绑定一个回调函数,用于响应的序列化和网络发送
    void SendMessage(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
};
