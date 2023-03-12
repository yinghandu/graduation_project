//
// Created by duyin on 2023/2/9.
//

#include "../header/PostOffice.h"

void PostOffice::init(const char *ip,const char *port) {
    Com::init(ip,port);
}

void PostOffice::schedulerCreateObjects(std::unordered_map<int,std::pair<std::string,std::string>> &ips) {
    std::unordered_map<int,std::pair<std::string,std::string>>::iterator it;
    for(it=ips.begin();it!=ips.end();++it){
        Com *com=new Com();
        com->id=it->first;
        char *ip=new char[strlen(it->second.first.c_str())+1],*port=new char[strlen(it->second.second.c_str())+1];
        strcpy(ip,it->second.first.c_str());
        strcpy(port,it->second.second.c_str());
        // com->setSendAddr(ip,port);
        // while(!com->sendSocket());
        // coms.push_back(com);
        std::cout<<"开始连接"<<std::endl;
    }
}

bool PostOffice::schedulerCreatePassiveConnections() {
    int alla=0,iResult;
    u_long iMode = 0;
    std::cout<<"coms size is:"<<coms.size()<<std::endl;
    while(true){
        struct sockaddr_in recv_addr;
        socklen_t len=sizeof(recv_addr);
        SOCKET receive;
        std::cout<<"begin accept"<<std::endl;
        receive=accept4(Com::listenSock,(sockaddr*)&recv_addr, &len,SOCK_NONBLOCK);//设置为非阻塞
        std::cout<<"accept success"<<std::endl;
        //get receive ip
        char *tmpIp=inet_ntoa(recv_addr.sin_addr);//转网络字节序
        std::cout<<"ip is :"<<tmpIp<<std::endl;
        for(int i=0;i<coms.size();++i){
            if(!coms[i]->allocated){
                if(!strcmp(tmpIp,coms[i]->ip)){
                    std::cout<<"found!"<<std::endl;
                    coms[i]->recvSock=receive;
                    coms[i]->recvAddr=recv_addr;
                    coms[i]->allocated=true;
                    alla++;
                }
            }
        }
        if(alla==coms.size()) break;
    }
}

void PostOffice::server_workerConnectScheduler(const char * ip_,const char *port_) {
    while(true){
        struct sockaddr_in recv_addr;
        socklen_t len=sizeof(recv_addr);
        SOCKET receive;
        std::cout<<"begin accept"<<std::endl;
        receive=accept4(Com::listenSock,(sockaddr*)&recv_addr, &len,SOCK_NONBLOCK);
        std::cout<<"accept success"<<std::endl;
        //get receive ip
        char *tmpIp=inet_ntoa(recv_addr.sin_addr);
        char ip[16],port[5];
        strcpy(ip,ip_);
        strcpy(port,port_);
        if(!strcmp(tmpIp,ip)){
            Com *com=new Com();
            strcpy(com->ip,ip);
            com->id=0;
            com->recvSock=receive;
            com->setSendAddr(ip,port);
            com->sendSocket();
            coms.push_back(com);
            //connect success
            break;
        }else{
            close(receive);
        }
    }
}



