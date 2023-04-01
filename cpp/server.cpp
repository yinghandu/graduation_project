//
// Created by duyin on 2023/2/7.
//

#include "../header/server.h"

void server::init(){
    loadConfig(data->config_path);  //load config;
    post_office->init(data->ip.data(),data->port.data());//set local ip and port ,begin listening
    post_office->server_workerConnectScheduler(__nodes.find(0)->second.first.data(),__nodes.find(0)->second.second.data());
    printf("server初始化成功,启动recv_thread和send_thread");
    std::thread recv_thread(&PostOffice::receiveMessage, post_office);
    recv_thread.detach();
    std::thread send_thread(&PostOffice::sendMessage, post_office);
    send_thread.detach();
}