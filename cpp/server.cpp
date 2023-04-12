//
// Created by duyin on 2023/2/7.
//

#include "../header/server.h"

void server::init(){
    //tmp
    w.resize(16);
    for(int i=0;i<w.size();i++){
        w[i]=0;
    }

    post_office->begin=false;
    loadConfig(datas->config_path);  //load config;
    post_office->init(datas->ip.data(),datas->port.data());//set local ip and port ,begin listening
    post_office->server_workerConnectScheduler(__nodes.find(0)->second.first.data(),__nodes.find(0)->second.second.data());
    printf("server初始化成功,启动recv_thread和send_thread");
    std::thread recv_thread(&PostOffice::receiveMessage, post_office);
    recv_thread.detach();
    std::thread send_thread(&PostOffice::sendMessage, post_office);
    send_thread.detach();
}

void server::waitBegin() const {
    while(!post_office->begin);
}

void server::maintainWeights(){
    while(true){
        recv_mtx.lock(); // lock
        if (!recv_queue.empty()) { 
            // std::cout<<"recv_queue不为空: "<<std::endl;
            packs * p = recv_queue.front(); //get a packet from queue
            recv_queue.pop(); 
            recv_mtx.unlock(); // unlock
            if(p->meta==Option::PUSH){
                // std::cout<<"接收到  worker "<<p->send_id<<" 的 PUSH 请求"<<std::endl;
                dealWithPush(p->weights);
            }else if(p->meta==Option::PULL){
                // std::cout<<"接收到  worker "<<p->send_id<<" 的 PULL 请求"<<std::endl;
                dealWithPull(p->send_id);
            }
            p->weights.clear();
            delete p;
            p = nullptr;
        } else {
            recv_mtx.unlock(); 
        }

    }
}

void server::dealWithPush(std::vector<float> & weights){
    // std::cout<<"原始参数是： "<<std::endl;
    // for(int i=0;i<weights.size();i++){
    //     std::cout<<w[i]<<" ";
    // }
    // std::cout<<std::endl;

    // std::cout<<"接收到的参数是： "<<std::endl;

    // for(int i=0;i<weights.size();i++){
    //     std::cout<<weights[i]<<" ";
    // }
    // std::cout<<std::endl;

    // std::cout<<datas->alpha<<std::endl;
    for(int i=0;i<weights.size();i++){
        w[i]-=weights[i]*datas->alpha;
    }

    // std::cout<<"计算后的参数是： "<<std::endl;
    // for(int i=0;i<weights.size();i++){
    //     std::cout<<w[i]<<" ";
    // }
    // std::cout<<std::endl;
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
