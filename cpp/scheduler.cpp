//
// Created by duyin on 2023/2/7.
//

#include "../header/scheduler.h"

void scheduler::init() {
    loadConfig(data->config_path);  //load config;
    post_office->setId(0);
    post_office->init(data->ip.data(),data->port.data());//set local ip and port ,begin listening
    post_office->schedulerCreateObjects(__nodes);//scheduler actively initiate connections (message from __nodes);
    post_office->schedulerCreatePassiveConnections();//scheduler passively receive incoming connections
    printf("scheduler初始化成功,启动recv_thread和send_thread");
    for(int i=0;i<post_office->servers.size();i++){
        for(int j=0;j<post_office->workers.size();j++){
            std::string sw=std::to_string(post_office->servers[i])+" "+std::to_string(post_office->workers[j]);
            post_office->sw_connected.insert({sw,0});
        }
    }
    std::thread recv_thread(&PostOffice::receiveMessage, post_office);
    recv_thread.detach();
    std::thread send_thread(&PostOffice::sendMessage, post_office);
    send_thread.detach();
    post_office->schedulerSetID();
    post_office->schedulerConfirmID();
    post_office->schedulerSendServerWorker();
    post_office->schedulerConfirmConnection();
    std::cout<<"ALL INITIATED!"<<std::endl;
}



