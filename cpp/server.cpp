//
// Created by duyin on 2023/2/7.
//

#include "../header/server.h"

void server::init(){
    loadConfig(data->config_path);  //load config;
    post_office->init(data->ip.data(),data->port.data());//set local ip and port ,begin listening
    post_office->server_workerConnectScheduler(__nodes.find(0)->second.first.data(),__nodes.find(0)->second.second.data());
    std::cout<<"与scheduler连接成功!"<<std::endl;
}