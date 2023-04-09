//
// Created by duyin on 2023/2/7.
//

#include "../header/scheduler.h"

void scheduler::init() {
    loadConfig(datas->config_path);  //load config;
    post_office->setId(0);
    post_office->init(datas->ip.data(),datas->port.data());//set local ip and port ,begin listening
    printf("scheduler初始化成功,尝试与server和worker建立连接\n");
    post_office->schedulerCreateObjects(__nodes);//scheduler actively initiate connections (message from __nodes);
    post_office->schedulerCreatePassiveConnections();//scheduler passively receive incoming connections
    printf("连接建立成功，通知server worker的信息\n");
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
    printf("通知各结点的ID信息");
    post_office->schedulerConfirmID();
    post_office->schedulerSendServerWorker();
    post_office->schedulerConfirmConnection();
    std::cout<<"所有结点初始化完成"<<std::endl;
}

void scheduler::beginWorking() {
    for(int i=0;i<post_office->servers.size();i++){
        packs *p=new packs();
        p->meta=Option::BEGIN;
        p->recv_id=post_office->servers[i];
        p->send_id=0;
        send_mtx.lock(); // lock
        send_queue.push(p);
        send_mtx.unlock(); // unlock
    }
    for(int i=0;i<post_office->workers.size();i++){
        packs *p=new packs();
        p->meta=Option::BEGIN;
        p->recv_id=post_office->workers[i];
        p->send_id=0;
        send_mtx.lock(); // lock
        send_queue.push(p);
        send_mtx.unlock(); // unlock
    }
}



