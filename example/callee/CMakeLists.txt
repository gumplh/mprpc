set(SRC_LIST userservice.cc ../user.pb.cc)
add_executable(provider ${SRC_LIST})
link_directories(provider -lprotobuf)