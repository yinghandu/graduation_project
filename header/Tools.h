//
// Created by duyin on 2023/3/19.
//

#ifndef TOOLS_H
#define TOOLS_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // !_GNU_SOURCE
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <utility>
#include <cstdio>
#include <thread>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <random>
#include <cstring>
#include <thread>
#include <poll.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <fcntl.h>

typedef int SOCKET;//typedef

enum class Option {
    EMPTY,
    SET_ID,
    CONFIRM_ID,
    SERVER_WORKER_CONNECTED,
    CONFIRM_CONNECTED,
    BEGIN,
    PUSH,
    PULL
};

#pragma pack(push, 1)
typedef struct Packet {
    Option meta;//option
    int send_id;//send id
    int recv_id;//recv id
    std::vector<float> weights;//weights
    std::string w;//string weights
    std::string msg;//message
}packs;
#pragma pack(pop)


const char DELIMITER = '\n';//deal with tcp packet

extern std::queue<packs *> send_queue;

extern std::queue<packs *> recv_queue;

extern std::mutex send_mtx;

extern std::mutex recv_mtx;

inline std::string transfer(std::vector<float> & weights);

inline void serialize(packs& p, char* buffer, size_t buffer_size);

inline void deserialize(char* buffer, size_t buffer_size, packs& p);

inline std::string transfer(std::vector<float> & weights){
    std::ostringstream ss;
    for (const auto& weight : weights) {
        ss << std::fixed << std::setprecision(4) << weight << " ";
    }
    std::string str = ss.str();
    return str;
}

inline void serialize(packs& p, char* buffer, size_t buffer_size) {
    // std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
    size_t offset = 0;
    memcpy(buffer + offset, &p.meta, sizeof(p.meta));
    offset += sizeof(p.meta);
    // std::cout << "send set p.meta " << offset << std::endl;
    memcpy(buffer + offset, &p.send_id, sizeof(p.send_id));
    offset += sizeof(p.send_id);
    // std::cout << "send set p.send_id " << p.send_id << std::endl;
    memcpy(buffer + offset, &p.recv_id, sizeof(p.recv_id));
    offset += sizeof(p.recv_id);
    // std::cout << "send set p.recv_id " << p.recv_id << std::endl;
    
    // int weights_num = p.weights.size();
    // memcpy(buffer + offset, &weights_num, sizeof(weights_num));
    // offset += sizeof(weights_num);
    // for (int i = 0; i < weights_num; i++) {
    //     memcpy(buffer + offset, &p.weights[i], sizeof(p.weights[i]));
    //     offset += sizeof(p.weights[i]);
    // }
    // Serialize weights
    int w_length = p.w.length()+1;
    // std::cout << "send w_length " << w_length << std::endl;
    memcpy(buffer + offset, &w_length, sizeof(w_length));
    offset += sizeof(w_length);
    // std::cout << "send set w_length " << offset << std::endl;
    memcpy(buffer + offset, p.w.c_str(), w_length+1);
    offset += w_length+1;//复制了'\0'
    // std::cout << "send set w " << offset << std::endl;
    int msg_length = p.msg.length();
    memcpy(buffer + offset, &msg_length, sizeof(msg_length));
    offset += sizeof(msg_length);
    // std::cout << "send set msg_length " << msg_length << std::endl;
    // std::cout<<"send msg是: "<<p.msg<<std::endl;
    memcpy(buffer + offset, p.msg.c_str(), msg_length+1);
    offset += msg_length+1;
    // std::cout << "send set msg " << offset << std::endl;
    memcpy(buffer + offset, &DELIMITER, sizeof(char));
    offset += sizeof(char);
    buffer[buffer_size]='\0';
    // std::cout << "send set DELIMITER " << offset << std::endl;
    if (offset != buffer_size) {
        std::cerr << "Error: serialized data size mismatch" << std::endl;
    }
    // else{
    //     std::cout<<"send "<<offset<<" 个"<<std::endl;
    // }
    // std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
}

inline void deserialize(const char* buffer, size_t buffer_size, packs& p) {
    // std::cout<<"======================================================"<<std::endl;
    size_t offset = 0;
    memcpy(&p.meta, buffer + offset, sizeof(p.meta));
    offset += sizeof(p.meta);
    // std::cout << "recv set p.meta " << offset << std::endl;
    memcpy(&p.send_id, buffer + offset, sizeof(p.send_id));
    offset += sizeof(p.send_id);
    // std::cout << "recv set p.send_id " << offset << std::endl;
    memcpy(&p.recv_id, buffer + offset, sizeof(p.recv_id));
    offset += sizeof(p.recv_id);
    // std::cout << "recv set p.recv_id " << offset << std::endl;
    // std::cout<<"send_id是 "<<p.send_id<<" recv id 是: "<<p.recv_id<<std::endl;
    // int weights_num = 0;
    // memcpy(&weights_num, buffer + offset, sizeof(weights_num));
    // offset += sizeof(weights_num);
    // for (int i = 0; i < weights_num; i++) {
    //     float weight = 0;
    //     memcpy(&weight, buffer + offset, sizeof(weight));
    //     offset += sizeof(weight);
    //     p.weights.push_back(weight);
    // }
    int w_length = 0;
    memcpy(&w_length, buffer + offset, sizeof(w_length));
    offset += sizeof(w_length);
    // std::cout << "recv set w_length len " << offset << std::endl;
    char* w_buffer = new char[w_length + 1];
    memcpy(w_buffer, buffer + offset, w_length);
    w_buffer[w_length] = '\0';
    std::istringstream iss(w_buffer);
    float f;
    while (iss >> f) {
        p.weights.push_back(f);
    }
    offset+=w_length+1;
    // std::cout << "send set w_length " << offset << std::endl;
    // std::cout<<" w是 ========="<< w_buffer<<std::endl;
    delete[] w_buffer;
    int msg_length = 0;
    memcpy(&msg_length, buffer + offset, sizeof(msg_length));
    offset += sizeof(msg_length);   
    // std::cout<<"msg length是: "<<msg_length<<std::endl;
    // std::cout << "recv set msg_length  len" << offset << std::endl;
    char* msg_buffer = new char[msg_length + 1];
    memcpy(msg_buffer, buffer + offset, msg_length);
    msg_buffer[msg_length] ='\0';
    offset+=msg_length+1;
    // std::cout << "recv set msg_length " << offset << std::endl;
    p.msg=msg_buffer;
    delete[] msg_buffer;
    if (offset != buffer_size) {
        // std::cout<<"offset是： "<<offset<<" buffer_size是 "<<buffer_size<<std::endl;
        std::cout << "Error: deserialized data size mismatch" << std::endl;
    }
    // else{
    //     std::cout<<"recv "<<offset<<" 个"<<std::endl;
    // }
    // std::cout<<"=============================================="<<std::endl;
}

inline float sigmoid(float x)
{
    return  1 / (1 + exp(-x));
}

inline float loss_compute(float x, float y)
{
    float loss;
    loss = y * log(x) + (1 - y)*log(1 - x);
    return loss;
}

#endif //TOOLS_H
