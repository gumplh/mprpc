#include "rpcprovider.h"
#include "google/protobuf/descriptor.h"
#include "rpcheader.pb.h"
/*
service_name 对应一个 service 描述， 一个service描述对应一个服务对象
一个method_name对应一个method方法
*/
//外部代码使用，用于发布服务
//需要生成一张表，记录服务对象和其发布的方法
void RpcProvider::NotifyService(google::protobuf::Service *service){
    ServiceInfo serviceinfo;
    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *psd = service->GetDescriptor();
    //获取服务的名字
    const std::string& service_name = psd -> name();
    int methodCnt = psd -> method_count();
    for(int i = 0; i < methodCnt; i++){
        const google::protobuf::MethodDescriptor *pmethodDes = psd -> method(i);
        serviceinfo.m_methodMap.insert({pmethodDes->name(),pmethodDes});
    }
    serviceinfo.m_service = service;
    m_ServiveInfoMap.insert({service_name,serviceinfo});
}


//启动rpc服务节点，提供rpc远程调用服务
void RpcProvider::Run(){
    std::string ip = MprpcApplication::GetIntance().GetConfig().Load("rpcserverip");
    std::string portStr = MprpcApplication::GetIntance().GetConfig().Load("rpcserverport");
    uint16_t port = atoi(portStr.c_str());
    muduo::net::InetAddress address(ip, port);
    
    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    //绑定连接回调和消息读写回调，很好的分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    
    //设置muduo库的线程数量，总线程数>=1，如果是一个线程，工作线程和IO线程就在一个线程里面，如果是多个线程，就会有一个IO线程负责网络连接，将连接分配个多个线程。
    server.setThreadNum(3);
    std::cout << "RpcProvider start service at ip:" << ip << ", port:" << port << std::endl;
    //启动网络服务
    server.start();
    m_eventLoop.loop();
}

//新的socket连接
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn){
    if(!conn->connected()){
        conn->shutdown();
    }
}

/*
在框架内部，服务提供者和接收者协商好之间通信用protobuf数据类型
需要：service_name、method_name、args，定义proto message类型进行数据头的序列化和反序列
header_size(4个字节，除了方法参数，其他所有的数据，如所调用的服务，方法的名字) + header_str（服务名、方法名、参数长度） + args_str,需要处理粘包的问题（记录消息的大小）
proto的格式是：servicename、methodname、args的size
*/
//已建立连接的读写时间回调，如果远程有一个rpc服务的调用请求，那么message就会响应
//按内存处理字符串和整数的时候，会使用到insert和copy方法
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, 
                            muduo::net::Buffer *buffer,
                            muduo::Timestamp time){
    // 网络上接受的远程rpc调用的字符流 login args
    std::string recv_buf = buffer->retrieveAllAsString();
    //从字符流中读取前4个字节的内容
    uint32_t head_size = 0;
    recv_buf.copy((char*)head_size, 4, 0);

    //根据headsize读取数据头的原始字符流
    std::string rpc_head_str = recv_buf.substr(4, head_size);
    mprpc::RpcHeader rh;
    std::string service_name;
    std::string method_name;
    uint32_t argsSize = 0;
    if(rh.ParseFromString(rpc_head_str)){
        //数据头反序列化成功
        service_name = rh.service_name();
        method_name = rh.method_name();
        argsSize = rh.argssize();
    } else {
        //记录日志
        std::cout << "rpc_header_str parse error:" << rpc_head_str << std::endl;
        return;
    }
    std::string args_str = recv_buf.substr(4 + head_size, argsSize);

    //打印调试信息
    std::cout << "========================" << std::endl;
    std::cout << "head_size" << head_size << std::endl;
    std::cout << "service_name" << service_name << std::endl;
    std::cout << "method_name" << method_name << std::endl;
    std::cout << "argsSize" << argsSize << std::endl;
    std::cout << "args_str" << args_str << std::endl;
    std::cout << "========================" << std::endl;

    //获取service对象和method方法
    auto it = m_ServiveInfoMap.find(service_name);
    if(it == m_ServiveInfoMap.end()){
        std::cout << "no service" << std::endl;
        return;
    }
    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end()){
        std::cout <<"no method" << std::endl;
        return;
    }
    google::protobuf::Service *service = it->second.m_service;
    const google::protobuf::MethodDescriptor *method = mit->second;
    
    //生成请求request和响应reponse

    google::protobuf::Message *request = service -> GetRequestPrototype(method).New();
    if(request->ParseFromString(args_str)){
        std::cout << "parse err" << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service -> GetResponsePrototype(method).New();
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider,
                                                                    const muduo::net::TcpConnectionPtr&,
                                                                    google::protobuf::Message*>
                                                                    (this, &RpcProvider::SendMessage, conn, response);
    //在框架上调用方法
    service->CallMethod(method, NULL, request, response, done);
}

//给closure绑定一个回调函数,用于响应的序列化和网络发送
void RpcProvider::SendMessage(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
    std::string response_str;
    if(response->SerializeToString(&response_str)){
        //序列化成功
        conn->send(response_str);
    } else {
        std::cout << "SerializeToString err" << std::endl;
    }
    conn->shutdown();

}