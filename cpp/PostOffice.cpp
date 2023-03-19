//
// Created by duyin on 2023/2/9.
//

#include "../header/PostOffice.h"

void PostOffice::init(const char *ip,const char *port) {
    Com::init(ip,port);
}

void PostOffice::schedulerCreateObjects(std::unordered_map<int,std::pair<std::string,std::string>> &ips) {
    std::unordered_map<int,std::pair<std::string,std::string>>::iterator it;
    
    int _com_last;//记录coms的数量

    for(it=ips.begin();it!=ips.end();++it){
        coms.push_back(std::make_unique<Com>());
        _com_last=coms.size()-1;
        coms[_com_last]->id=it->first;
        char *ip=new char[strlen(it->second.first.c_str())+1],*port=new char[strlen(it->second.second.c_str())+1];
        strcpy(ip,it->second.first.c_str());
        strcpy(port,it->second.second.c_str());
        coms[_com_last]->setSendAddr(ip,port);
        while(!coms[_com_last]->sendSocket());
        delete ip;
        delete port;
    }

}

bool PostOffice::schedulerCreatePassiveConnections() {
    int alla=0,iResult;
    u_long iMode = 0;
    printf("coms size is:%d\n",coms.size());
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
    printf("scheduler initiated!\n");
    return true;
}

void PostOffice::server_workerConnectScheduler(const char * ip_,const char *port_) {
    int _com_last;//记录coms的数量
    while(true){
        struct sockaddr_in recv_addr;
        socklen_t len=sizeof(recv_addr);
        SOCKET receive;
        std::cout<<"begin accept"<<std::endl;
        receive=accept4(Com::listenSock,(sockaddr*)&recv_addr, &len,SOCK_NONBLOCK);
        std::cout<<"accept success"<<std::endl;
        //get receive ip

        char *tmpIp=inet_ntoa(recv_addr.sin_addr);
        
        char tmpPort[6];
        uint16_t ports = ntohs(recv_addr.sin_port);
        sprintf(tmpPort, "%hu", ports);

        //const cahr * to char *
        char ip[32],port[6];
        memset(ip, 0, sizeof(ip));
        memset(port, 0, sizeof(port));
        strcpy(ip,ip_);
        strcpy(port,port_);

        if(!strcmp(tmpIp,ip)){
            coms.push_back(std::make_unique<Com>());
            _com_last=coms.size()-1;
            strcpy(coms[_com_last]->ip,ip);
            coms[_com_last]->id=0;
            coms[_com_last]->recvSock=receive;
            coms[_com_last]->setSendAddr(ip,port);
            while(!coms[_com_last]->sendSocket());
            std::cout<<"Connect success!"<<std::endl;
            break;
        }else{
            std::cout<<"tmpIp is:"<<tmpIp<<std::endl;
            close(receive);
        }
    }

}



