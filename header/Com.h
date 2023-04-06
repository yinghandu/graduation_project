//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_COM_H 
#define DIS_COM_H 

#include "Tools.h"  

class Com {
private:
    char ip[32];
    char port[6];
    int id;
    bool allocated=false;
    bool idSetted=false;
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
    void setId(int Id);
    friend class PostOffice;
    friend class nodes;
};


#endif //DIS_COM_H
