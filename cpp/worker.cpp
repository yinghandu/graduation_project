//
// Created by duyin on 2023/2/7.
//

#include "../header/worker.h"

void worker::init(){
    begin=false;
    loadConfig(data->config_path);  //load config;
    post_office->init(data->ip.data(),data->port.data());//set local ip and port ,begin listening
    post_office->server_workerConnectScheduler(__nodes.find(0)->second.first.data(),__nodes.find(0)->second.second.data());
    printf("worker初始化成功,启动recv_thread和send_thread");
    std::thread recv_thread(&PostOffice::receiveMessage, post_office);
    recv_thread.detach();
    std::thread send_thread(&PostOffice::sendMessage, post_office);
    send_thread.detach();
}

void worker::begin() const {
    while(!begin);
}

void worker::random_sample(const std::vector<std::vector<float>>& data,std::vector<std::vector<float>>& samples ,int num_samples){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> diss(0, data.size() - 1);
    for (int i = 0; i < num_samples; ++i) {
        int idx = diss(gen);
        samples.push_back(data[idx]);
    }
}

void worker::pull(int ID){
    packs *p=new packs();
    p->meta=Option::PULL;
    p->recv_id=ID;
    p->send_id=id;
    send_mtx.lock(); // lock
    send_queue.push(p);
    send_mtx.unlock(); // unlock

    while(true){
        recv_mtx.lock(); // lock
        if (!recv_queue.empty()) { 
            packs * p = recv_queue.front(); //get a packet from queue
            recv_queue.pop(); 
            recv_mtx.unlock(); // unlock
            w=p->weights;
            delete[] p->weights.data();
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
    p->send_id=id;
    p->weights=w;
    send_mtx.lock(); // lock
    send_queue.push(p);
    send_mtx.unlock(); // unlock
}