//
// Created by duyin on 2023/2/9.
//

#ifndef DIS_POSTOFFICE_H
#define DIS_POSTOFFICE_H
#include "Com.h"
#include <cstring>
#include <thread>

const int maxCom = 10;

class PostOffice
{
private:
    int id;
    bool rece_end = false;
    bool send_end = false;
    bool initiated = false;
    std::vector<std::unique_ptr<Com>> coms;

public:
    PostOffice(){};
    void init(const char *ip, const char *port); // parameter: local ip and port
    void schedulerCreateObjects(std::unordered_map<int, std::pair<std::string, std::string>> &ips);
    bool schedulerCreatePassiveConnections();
    void server_workerConnectScheduler(const char *ip_, const char *port_);
    void receiveMessage();
};

#endif // DIS_POSTOFFICE_H

// data:
//     [0]:com id
//     [1]:listen ip
//     [2]:listen port
//     [3]:send ip
//     [4]:send port