//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_COM_H 
#define DIS_COM_H 
#ifndef _GNU_SOURCE
#define _GNU_SOURC
#endif // !_GNU_SOURCE
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <unordered_map>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <random>


extern std::queue<std::pair<int,std::string>> mq;

typedef struct packet
{
    int msgLen ;  //4个字节字段，说明数据部分的大小
    const char *data;
}buff;

typedef int SOCKET;

class Com {
private:
    char ip[16];
    int id;
    bool allocated=false;
    struct sockaddr_in sendAddr;
    struct sockaddr_in recvAddr;
    static struct sockaddr_in listenAddr;
    static SOCKET listenSock;
    SOCKET sendSock;
    SOCKET recvSock;

public:
    Com() {
        id = 0;
        allocated = false;
        memset(ip, 0, sizeof(ip)); //清空ip数组
        memset(&sendAddr, 0, sizeof(sendAddr)); //清空sendAddr结构体
        memset(&recvAddr, 0, sizeof(recvAddr)); //清空recvAddr结构体
        sendSock = -1; //初始化sendSock为无效值
        recvSock = -1; //初始化recvSock为无效值
    }

    Com(const Com& other); // 深拷贝构造函数声明
    static void init(const char *ip,const char *port); //initiate listen port
    void setSendAddr(char *Ip,char *Port); //set send addr
    bool sendSocket();//sendsocket
    bool recvSocket();//recvsocket
    bool sendMessage(std::string &data);
    bool recvm(std::string &data);
    void setId(int Id);
    friend class PostOffice;
};


#endif //DIS_COM_H
