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
#include <cmath>
#include <fcntl.h>

typedef int SOCKET;//typedef

enum class Option {
    SET_ID,
    CONFIRM_ID,
    SERVER_WORKER_CONNECTED,
    CONFIRM_CONNECTED,
    PUSH,
    PULL
};

#pragma pack(push, 1)
typedef struct Packet {
    Option meta;//option
    int send_id;//send id
    int recv_id;//recv id
    // int weights_num;
    // int msg_length;
    std::vector<int> weights;//weights

    std::string msg;//message
}packs;
#pragma pack(pop)


const char DELIMITER = '\n';//deal with tcp packet

extern std::queue<packs *> send_queue;

extern std::queue<packs *> recv_queue;

extern std::mutex send_mtx;

extern std::mutex recv_mtx;

inline void serialize(packs& p, char* buffer, size_t buffer_size);

inline void deserialize(char* buffer, size_t buffer_size, packs& p);

inline void serialize(packs& p, char* buffer, size_t buffer_size) {
    size_t offset = 0;
    memcpy(buffer + offset, &p.meta, sizeof(p.meta));
    offset += sizeof(p.meta);
    memcpy(buffer + offset, &p.send_id, sizeof(p.send_id));
    offset += sizeof(p.send_id);
    memcpy(buffer + offset, &p.recv_id, sizeof(p.recv_id));
    offset += sizeof(p.recv_id);
    int weights_num = p.weights.size();
    memcpy(buffer + offset, &weights_num, sizeof(weights_num));
    offset += sizeof(weights_num);
    for (int i = 0; i < weights_num; i++) {
        memcpy(buffer + offset, &p.weights[i], sizeof(p.weights[i]));
        offset += sizeof(p.weights[i]);
    }
    int msg_length = p.msg.length();
    memcpy(buffer + offset, &msg_length, sizeof(msg_length));
    offset += sizeof(msg_length);
    memcpy(buffer + offset, p.msg.c_str(), msg_length);
    offset += msg_length;
    memcpy(buffer + offset, &DELIMITER, sizeof(char));
    offset += sizeof(char);
    if (offset != buffer_size) {
        std::cerr << "Error: serialized data size mismatch" << std::endl;
    }
}

inline void deserialize(const char* buffer, size_t buffer_size, packs& p) {
    size_t offset = 0;
    memcpy(&p.meta, buffer + offset, sizeof(p.meta));
    offset += sizeof(p.meta);
    //std::cout<<"sizeof(p.meta)"<<sizeof(p.meta)<<std::endl;
    memcpy(&p.send_id, buffer + offset, sizeof(p.send_id));
    offset += sizeof(p.send_id);
    //std::cout<<"sizeof(p.send_id)"<<sizeof(p.send_id)<<std::endl;
    memcpy(&p.recv_id, buffer + offset, sizeof(p.recv_id));
    offset += sizeof(p.recv_id);
    //std::cout<<"sizeof(p.recv_id)"<<sizeof(p.recv_id)<<std::endl;
    int weights_num = 0;
    memcpy(&weights_num, buffer + offset, sizeof(weights_num));
    offset += sizeof(weights_num);
    //std::cout<<"weights_num"<<sizeof(weights_num)<<std::endl;
    for (int i = 0; i < weights_num; i++) {
        int weight = 0;
        memcpy(&weight, buffer + offset, sizeof(weight));
        offset += sizeof(weight);
        p.weights.push_back(weight);
    }
    int msg_length = 0;
    memcpy(&msg_length, buffer + offset, sizeof(msg_length));
    offset += sizeof(msg_length);
    //std::cout<<"sizeof(msg_length)"<<sizeof(msg_length)<<std::endl;
    char* msg_buffer = new char[msg_length + 1];
    memcpy(msg_buffer, buffer + offset, msg_length);
    msg_buffer[msg_length] = '\0';
    p.msg = msg_buffer;
    offset+=msg_length;
    delete[] msg_buffer;
    if (offset != buffer_size) {
        std::cerr << "Error: deserialized data size mismatch" << std::endl;
    }
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
