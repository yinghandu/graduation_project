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
    bool initiated = false;
    bool idSetted = false;

    std::vector<std::unique_ptr<Com>> coms;

    std::unordered_map<std::string,int> sw_connected;//only for scheduler,initialized in scheduler

public:
    PostOffice() : sw_connected() {}
    void setId(int Id);//set local id
    void showId() const;//show id
    void init(const char *ip, const char *port); // parameter: local ip and port
    void schedulerCreateObjects(std::unordered_map<int, std::pair<std::string, std::string>> &ips);//scheduler create objects
    bool schedulerCreatePassiveConnections();//scheduler create passive connections
    void server_workerConnectScheduler(const char *ip_, const char *port_);//servers or workers connect with scheduler
    void server_workerConnect(std::string msg);//workers or servers connect with each other
    void sendMessage();//send message based on the send_queue
    void receiveMessage();//receive the message from the socket and creat a packs and insert it into the recv_queue
    void showComsMessage() const ;//show coms messages
    void schedulerSetID();//inform the servers and workers their id
    void schedulerConfirmID();//confirm servers and workers have set their id
    void server_workerWaitID();//wait the scheduler inform id
    void serverInformConnect(std::string s_id,std::string w_id);//server inform the scheduler it had been connected with worker
    void schedulerSendServerWorker();//inform the servers of the ip and port they have to connect,or inform the workers
    void schedulerConfirmConnection();//inform the servers of the ip and port they have to connect,or inform the workers
    bool findID(int ids);

    std::vector<int> workers;
    std::vector<int> servers;
    int id;
    bool begin;

    friend class nodes;
    friend class scheduler;
};

#endif // DIS_POSTOFFICE_H

