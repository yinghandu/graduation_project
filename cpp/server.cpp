//
// Created by duyin on 2023/2/7.
//

#include "../header/server.h"

void server::init(){
    begin=false;
    loadConfig(data->config_path);  //load config;
    post_office->init(data->ip.data(),data->port.data());//set local ip and port ,begin listening
    post_office->server_workerConnectScheduler(__nodes.find(0)->second.first.data(),__nodes.find(0)->second.second.data());
    printf("server初始化成功,启动recv_thread和send_thread");
    std::thread recv_thread(&PostOffice::receiveMessage, post_office);
    recv_thread.detach();
    std::thread send_thread(&PostOffice::sendMessage, post_office);
    send_thread.detach();
}

void server::begin() const {
    while(!begin);
}

void server::maintainWeights(std::vector<float> & weights){
    while(true){
        recv_mtx.lock(); // lock
        if (!recv_queue.empty()) { 
            packs * p = recv_queue.front(); //get a packet from queue
            recv_queue.pop(); 
            recv_mtx.unlock(); // unlock
            
            if(p->meta==Option::PUSH){
                dealWithPush(p->weights);
            }else if(p->meta==Option::PULL){
                dealWithPull(p->send_id);
            }

            delete[] p->weights.data();
            p->weights.clear();
            delete p;
            p = nullptr;

        } else {
            recv_mtx.unlock(); 
        }

    }
}

void server::dealWithPush(std::vector<float> & weights){
    for(int i=0;i<weights.size();i++){
        w[i]-=weights[i]*alpha;
    }
}

void server::dealWithPull(int sendId){
    packs *p=new packs();
    p->meta=Option::PULL;
    p->recv_id=sendId;
    p->send_id=post_office->id;
    p->weights=w;
    send_mtx.lock(); // lock
    send_queue.push(p);
    send_mtx.unlock(); // unlock
}