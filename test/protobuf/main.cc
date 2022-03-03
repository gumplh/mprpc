#include <iostream>
#include <string>
#include "test.pb.cc"
using namespace fixbug;

int main(){
    //封装了login请求对象的数据，对象数据序列化
    LoginRequest req;
    req.set_name("zhang_san");
    req.set_pwd("123456");
    std::string send_str;
    if(req.SerializeToString(&send_str)){
        std::cout << send_str << std::endl;
    }

    //从sen_str反序列化一个请求对象
    LoginRequest reqB;
    if(reqB.ParseFromString(send_str)){
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }
}