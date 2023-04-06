//
// Created by duyin on 2023/2/7.
//

#include "../header/Com.h"

std::queue<packs *> send_queue;
std::queue<packs *> recv_queue;
std::mutex send_mtx;
std::mutex recv_mtx;

struct sockaddr_in Com::listenAddr;
SOCKET Com::listenSock;

Com::Com(const Com& other)
{
    std::strcpy(ip, other.ip);
    std::strcpy(port, other.port);
    id = other.id;
    allocated = other.allocated;
    std::memcpy(&sendAddr, &other.sendAddr, sizeof(sendAddr));
    std::memcpy(&recvAddr, &other.recvAddr, sizeof(recvAddr));
    listenSock = other.listenSock;
}

void Com::init(const char *ip,const char *port) {
    int res;
    memset(&listenAddr, 0, sizeof(listenAddr));  //set 0
    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    listenAddr.sin_family = AF_INET;  // use IPv4
    listenAddr.sin_addr.s_addr = inet_addr(ip);  //IPaddress
    char* p = (char*)malloc(strlen(port) + 1); // 分配足够的内存
    strcpy(p,port);
    listenAddr.sin_port = htons(std::atoi(p));  //port
    free (p);
    // int flags = fcntl(listenSock, F_GETFL, 0);
    // fcntl(listenSock, F_SETFL, flags | O_NONBLOCK);
    res=bind(listenSock,(struct sockaddr *)&listenAddr,sizeof(listenAddr));// 将socket()返回值和本地的IP端口绑定到一起
    if(res==-1) perror("BIND:");
    res=listen(listenSock,10);
    if(res==-1) perror("LISTEN:");
    std::cout<<"begin listening"<<std::endl;
}

void Com::setSendAddr(char *Ip,char *Port) {
    memset(&sendAddr, 0, sizeof(sendAddr));  //set 0
    // sendSock = socket(AF_INET, SOCK_STREAM, 0);
    sendAddr.sin_family = AF_INET;  // use IPv4
    sendAddr.sin_addr.s_addr = inet_addr(Ip);  //IP address
    sendAddr.sin_port = htons(std::stoi(Port));  //port
    strcpy(ip,Ip);
    strcpy(port,Port);
    // std::cout<<"ip seted"<<std::endl;
}

bool Com::sendSocket() {
    bool flag=true;
    sendSock=socket(AF_INET, SOCK_STREAM, 0);
    int len = sizeof(sockaddr_in);
    std::cout << "进入sendsocket"  << std::endl;

    int i = connect(sendSock, (sockaddr*)&sendAddr, len);
    perror("CONNECT");
    int flags = fcntl(sendSock, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(sendSock, F_SETFL, flags);
    std::cout << "Sending data to " << inet_ntoa(sendAddr.sin_addr) << ":" << ntohs(sendAddr.sin_port) << std::endl;
    if(i==-1){
        flag=false;
        perror("CONNECT");
    }
    else{
        std::cout<<"connect success!"<<std::endl;
    }
    
    return flag;
}

void Com::setId(int Id) {
    id=Id;
}
