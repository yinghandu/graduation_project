//
// Created by duyin on 2023/2/7.
//

#include "../header/Com.h"

std::queue<std::pair<int,std::string>> mq;


struct sockaddr_in Com::listenAddr;
SOCKET Com::listenSock;

Com::Com(const Com& other)
{
    std::strcpy(ip, other.ip);
    id = other.id;
    allocated = other.allocated;
    std::memcpy(&sendAddr, &other.sendAddr, sizeof(sendAddr));
    std::memcpy(&recvAddr, &other.recvAddr, sizeof(recvAddr));
    listenSock = other.listenSock;
}

void Com::init(const char *ip,const char *port) {
    memset(&listenAddr, 0, sizeof(listenAddr));  //set 0
    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    listenAddr.sin_family = AF_INET;  // use IPv4
    listenAddr.sin_addr.s_addr = inet_addr(ip);  //IPaddress
    char* p = (char*)malloc(strlen(port) + 1); // 分配足够的内存
    strcpy(p,port);
    listenAddr.sin_port = htons(std::atoi(p));  //port
    free (p);
    int res=bind(listenSock,(struct sockaddr *)&listenAddr,sizeof(listenAddr));// 将socket()返回值和本地的IP端口绑定到一起
    listen(listenSock,10);
    std::cout<<"begin listening"<<std::endl;
}

void Com::setSendAddr(char *Ip,char *Port) {
    memset(&sendAddr, 0, sizeof(sendAddr));  //set 0
    sendSock = socket(AF_INET, SOCK_STREAM, 0);
    sendAddr.sin_family = AF_INET;  // use IPv4
    sendAddr.sin_addr.s_addr = inet_addr(Ip);  //IP address
    sendAddr.sin_port = htons(std::stoi(Port));  //port
    strcpy(ip,Ip);
    std::cout<<"ip seted"<<std::endl;
}

bool Com::sendSocket() {
    bool flag=true;
    sendSock=socket(AF_INET, SOCK_STREAM, 0);
    int len = sizeof(sockaddr_in);
    int i = connect(sendSock, (sockaddr*)&sendAddr, len);
    std::cout<<"connect success"<<std::endl;
    std::cout<<"ip is:"<<ip<<std::endl;
    return flag;
}

bool Com::recvSocket() {
    // int len=sizeof(SOCKADDR);
//    recvSock=accept(Com::listenSock,(SOCKADDR*)&recvAddr, &len);
//    recvSock=WSAAccept(ListenSocket, (SOCKADDR*) &saClient, &iClientSize,
//                       &ConditionAcceptFunc, NULL);
    // return false;
}

void Com::setId(int Id) {
    id=Id;
}

bool Com::sendMessage(std::string &data) {
    // int iResult;
    // const char * d=data.c_str();
    // int n = strlen(d);   //calculate the length of Transmit Data
    // buff tmp;
    // tmp.msgLen= htonl(n);
    // tmp.data=d;
    // iResult = send(sendSock, reinterpret_cast<const char *>(&tmp), n + 4, 0 );
    // if (iResult == SOCKET_ERROR) {
    //     wprintf(L"send failed with error: %d\n", WSAGetLastError());
    //     exit(0);
    // }
    // data.clear();
    // return false;
}

bool Com::recvm(std::string &data) {
    // data.clear();
    // int iResult;
    // char msgLen[4];

    // iResult = recv(recvSock, msgLen, 4, MSG_PEEK);
    // if(iResult<0){
    //     return false;
    // }


    // int readBytes = recv(recvSock,recvbuf,,0); //读取出后续的数据


    // if ( iResult > 0 )
    //     printf("Bytes received: %d\n", iResult);
    // else if ( iResult == 0 )
    //     printf("Connection closed\n");
    // else
    //     printf("recv failed: %d\n", WSAGetLastError());


    // return false;
}




