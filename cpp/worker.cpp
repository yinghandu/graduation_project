//
// Created by duyin on 2023/2/7.
//

#include "../header/worker.h"

void worker::init(){
    post_office->begin=false;
    loadConfig(datas->config_path);  //load config;
    post_office->init(datas->ip.data(),datas->port.data());//set local ip and port ,begin listening
    post_office->server_workerConnectScheduler(__nodes.find(0)->second.first.data(),__nodes.find(0)->second.second.data());
    printf("worker初始化成功,启动recv_thread和send_thread");
    std::thread recv_thread(&PostOffice::receiveMessage, post_office);
    recv_thread.detach();
    std::thread send_thread(&PostOffice::sendMessage, post_office);
    send_thread.detach();
}

void worker::waitBegin() const {
    while(!post_office->begin);
}

void worker::random_sample(const std::vector<std::vector<float>>& data,std::vector<std::vector<float>>& samples ,int num_samples){
    std::random_device rd;
    unsigned int seed = rd();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> diss(0, data.size() - 1);
    samples.clear();
    for (int i = 0; i < num_samples; ++i) {
        int idx = diss(gen);
        samples.push_back(data[idx]);
    }
}

void worker::pull(int ID){
    packs *p=new packs();
    p->meta=Option::PULL;
    p->recv_id=ID;
    p->send_id=post_office->id;
    send_mtx.lock(); // lock
    send_queue.push(p);
    send_mtx.unlock(); // unlock

    while(true){
        recv_mtx.lock(); // lock
        if (!recv_queue.empty()) { 
            // std::cout<<"pull 到了数据"<<std::endl;
            packs * p = recv_queue.front(); //get a packet from queue
            recv_queue.pop(); 
            recv_mtx.unlock(); // unlock
            w=p->weights;
            p->weights.clear();
            delete p;
            p = nullptr;
            break;
        } else {
            recv_mtx.unlock(); 
        }
    } 
}

void worker::push(int ID){
    packs *p=new packs();
    p->meta=Option::PUSH;
    p->recv_id=ID;
    p->send_id=post_office->id;
    p->weights=w;
    send_mtx.lock(); // lock
    send_queue.push(p);
    send_mtx.unlock(); // unlock
}

void worker::serverKeepAlive(){
    std::cout<<"keep alive--server id is :"<<post_office->servers[0]<<std::endl;
    packs *p=new packs();
    p->meta=Option::EMPTY;
    p->recv_id=post_office->servers[0];
    p->send_id=post_office->id;
    send_mtx.lock(); // lock
    send_queue.push(p);
    send_mtx.unlock(); // unlock
}