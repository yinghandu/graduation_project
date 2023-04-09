//
// Created by duyin on 2023/2/9.
//

#include "../header/PostOffice.h"



void PostOffice::setId(int Id) {
    id=Id;
}

void PostOffice::showId() const{
    std::cout<<"id 是:"<<id<<std::endl;
}

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
        if(it->first%2==1) servers.push_back(it->first);//记录workers id
        else workers.push_back(it->first);
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
    // printf("coms size is:%d\n",coms.size());
    while(true){
        struct sockaddr_in recv_addr;
        socklen_t len=sizeof(recv_addr);
        SOCKET receive;
        // std::cout<<"begin accept"<<std::endl;
        receive=accept4(Com::listenSock,(sockaddr*)&recv_addr, &len,SOCK_NONBLOCK);//设置为非阻塞
        // std::cout<<"accept success! receive的sock是: "<<receive<<std::endl;
        //get receive ip

        char *tmpIp=inet_ntoa(recv_addr.sin_addr);

        // std::cout<<"ip is :"<<tmpIp<<std::endl;

        for(int i=0;i<coms.size();++i){
            if(!coms[i]->allocated){
                if(!strcmp(tmpIp,coms[i]->ip)){
                    // std::cout<<"found!"<<std::endl;
                    coms[i]->recvSock=receive;
                    coms[i]->recvAddr=recv_addr;
                    coms[i]->allocated=true;
                    alla++;
                    break;
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
        SOCKET receive=-1;
        receive=accept4(Com::listenSock,(sockaddr*)&recv_addr, &len,SOCK_NONBLOCK);
        if (receive == -1 && errno == EAGAIN) {
                // 没有数据可用，等待下一次循环
                continue;
        }
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
            // std::cout<<"kai shi send"<<std::endl;
            while(!coms[_com_last]->sendSocket());
            std::cout<<"Connect success!"<<std::endl;
            break;
        }else{
            // std::cout<<"tmpIp is:"<<tmpIp<<std::endl;
            close(receive);
        }
    }
}

void PostOffice::server_workerConnect(std::string msg){
    std::string ip,port,server_id,worker_id;
    std::istringstream ss;
    ss.str(msg);
    ss >> ip;
    ss >> port;
    ss >> server_id;
    ss >> worker_id;

    char* ip_ = new char[ip.length() + 1];
    char* port_ = new char[port.length() + 1];
    // strcpy(tmpIp, ip.c_str());

    strcpy(ip_, "172.21.194.66");
    strcpy(port_, port.c_str());
    int _com_last;//记录coms的数量
    if(id%2==0){//worker
        std::cout<<"与server "<<server_id<<" 建立连接"<<std::endl;
        servers.push_back(std::stoi(server_id));
        while(true){
            struct sockaddr_in recv_addr;
            socklen_t len=sizeof(recv_addr);
            SOCKET receive=-1;
            receive=accept4(Com::listenSock,(sockaddr*)&recv_addr, &len,SOCK_NONBLOCK);
            if (receive == -1 && errno == EAGAIN) {
                    // 没有数据可用，等待下一次循环
                    continue;
            }
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
                coms[_com_last]->id=std::stoi(server_id);
                coms[_com_last]->recvSock=receive;
                coms[_com_last]->setSendAddr(ip,port);
                while(!coms[_com_last]->sendSocket());
                // std::cout<<"worker堆server的sendsock是："<<coms[_com_last]->sendSock<<std::endl;
                break;
            }else{
                // std::cout<<"tmpIp is:"<<tmpIp<<std::endl;
                close(receive);
            }
        }
    }else{//server
        std::cout<<"与worker "<<worker_id<<" 建立连接"<<std::endl;
        workers.push_back(std::stoi(worker_id));
        coms.push_back(std::make_unique<Com>());
        _com_last=coms.size()-1;
        coms[_com_last]->id=std::stoi(worker_id);
        workers.emplace_back(std::stoi(worker_id));
        coms[_com_last]->setSendAddr(ip_,port_);
        sleep(2);
        while(!coms[_com_last]->sendSocket());
        while(true){
            struct sockaddr_in recv_addr;
            socklen_t len=sizeof(recv_addr);
            SOCKET receive;
            receive=accept4(Com::listenSock,(sockaddr*)&recv_addr, &len, SOCK_NONBLOCK);//设置为非阻塞
            if (receive == -1 && errno == EAGAIN) {
                // 没有数据可用，等待下一次循环
                continue;
            }
            char *tmpip=inet_ntoa(recv_addr.sin_addr);
            if(!strcmp(ip_,tmpip)){
                coms[_com_last]->recvSock=receive;
                coms[_com_last]->recvAddr=recv_addr;
                coms[_com_last]->allocated=true;
                coms[_com_last]->id=std::stoi(worker_id);
                break;
            }
        }
        serverInformConnect(server_id,worker_id);

        // std::cout<<"server_id "<<server_id<<"  worker_id "<<worker_id<<std::endl;
        
    }
}

void PostOffice::sendMessage() {
    printf("Enter send thread!\n");
    while(true){
        send_mtx.lock(); // lock
        if (!send_queue.empty()) { 
            // std::cout<<"sendMessage()"<<std::endl;
            packs * p = send_queue.front(); // get a packet from queue
            send_queue.pop(); 
            send_mtx.unlock(); // unlock
            //packet send message
            p->w=transfer(p->weights);
            const size_t buffer_size = sizeof(Option) + 4 * sizeof(int) + p->w.length()+p->msg.length() + 2;//'\n'
            char* buffer = new char[buffer_size];
            serialize(*p, buffer, buffer_size);//Byte serialization 

            for (auto iter = coms.begin(); iter != coms.end(); ++iter) {
                // std::cout<<"here"<<std::endl;
                Com* com = iter->get();
                // std::cout<<com->id<<std::endl;
                // std::cout<<p->recv_id<<std::endl;
                // std::cout<<" ! "<<std::endl;
                if(com->id!=p->recv_id) continue;
                else{
                    //send
                    // std::cout<<"等待发送"<<std::endl;
                    // std::cout<<"com id 是 "<<com->id<<std::endl;
                    // std::cout<<"recv id 是"<<p->recv_id<<std::endl;
                    // std::cout<<"com recvsock 是"<<com->recvSock<<std::endl;
                    // std::cout<<"com sendsock 是"<<com->sendSock<<std::endl;
                    while(!send(com->sendSock, buffer, buffer_size, 0));
                    // std::cout << "Send a packet: " << buffer_size << " bytes" << std::endl;
                    break;
                }
            }
            p->weights.clear();
            delete p;
            p = nullptr;
        } else {
            send_mtx.unlock(); 
        }
    }
}

void PostOffice::receiveMessage(){
    printf("Enter receive thread!\n");
    std::string buffer;
    //use function: poll() to manage recvSock 
    std::vector<pollfd> pollFdVec(coms.size()); 

    for (size_t i = 0; i < coms.size(); i++) {
            pollFdVec[i].fd = coms[i]->recvSock;
            pollFdVec[i].events = POLLIN;
            pollFdVec[i].revents = 0;
    }

    int timeout=500;//ms

    while(true){
        int ret = poll(pollFdVec.data(), pollFdVec.size(), timeout);//moniter the recv message
        if (ret < 0) {
            printf("poll函数出错！\n");
            return ;
        } else if (ret == 0) {
            continue;
        } else {
            for (size_t i = 0; i < pollFdVec.size(); i++) {
                if (pollFdVec[i].revents & POLLIN) {//找到是哪个发送了
                    buffer.clear();
                    char recvbuf[2048];
                    int recvlen = recv(coms[i]->recvSock, recvbuf, sizeof(recvbuf), 0);
                    //std::cout<<" 他的recvsock是 "<< coms[i]->recvSock <<"它的recvlen是"<<recvlen<<std::endl;
                    if (recvlen < 0) {
                        perror("RECEIVE");
                        continue;
                    }
                    buffer.append(recvbuf, recvlen);
                    size_t pos;
                    while ((pos = buffer.find(DELIMITER)) != std::string::npos) {
                        std::string _packet = buffer.substr(0, pos);
                        buffer.erase(0, pos + 1);
                        const char* temp = _packet.c_str();
                        packs *p=new packs();
                        size_t buffer_size=_packet.size();
                        deserialize(temp, buffer_size, *p);
                        //std::cout << "Receive a packet: " << _packet.size() << " bytes" << std::endl;
                        p->send_id=coms[i]->id;
                        switch (p->meta)
                        {
                        case Option::SET_ID:
                            setId(std::stoi(p->msg));
                            server_workerWaitID();
                            break;
                        case Option::CONFIRM_ID:
                            coms[i]->idSetted=true;
                            break;
                        case Option::SERVER_WORKER_CONNECTED:
                            server_workerConnect(p->msg); 
                            break;
                        case Option::CONFIRM_CONNECTED:
                            if(sw_connected.find(p->msg) != sw_connected.end()){
                                sw_connected.find(p->msg)->second=1;
                                // std::cout<<"RIGHT"<<std::endl;
                            }
                            break;
                        case Option::BEGIN:
                            pollFdVec.resize(coms.size());
                            for (size_t i = 0; i < coms.size(); i++) {
                                pollFdVec[i].fd = coms[i]->recvSock;
                                pollFdVec[i].events = POLLIN;
                                pollFdVec[i].revents = 0;
                            }
                            begin=true;
                            break;
                        case Option::EMPTY:
                            break;
                        default:
                            // std::cout<<"default"<<std::endl;
                            recv_mtx.lock(); // lock
                            recv_queue.push(p);
                            recv_mtx.unlock(); // unlock
                            break;
                        }
                    }
                }
            }
        }
    }
}

void PostOffice::showComsMessage() const {
    printf("=================================================");
    std::cout<<"目前共连接 "<<coms.size()<<" 个对象!"<<std::endl;
    for (auto iter = coms.begin(); iter != coms.end(); ++iter) {
        Com* com = iter->get();
        std::cout<<"id: "<<com->id<<"   ip: " <<com->ip<<"    port: "<<com->port<<"  sendSock="<<com->sendSock<<" recvSock="<<com->recvSock<<std::endl;
    }
}

void PostOffice::schedulerSetID() {
    for (auto iter = coms.begin(); iter != coms.end(); ++iter) {
        Com* com = iter->get();
        packs *p=new packs();
        p->meta=Option::SET_ID;
        p->msg+=std::to_string(com->id);
        p->recv_id=com->id;
        p->send_id=id;
        send_mtx.lock(); // lock
        send_queue.push(p);
        send_mtx.unlock(); // unlock
    }
}

void PostOffice::schedulerSendServerWorker(){
    for (auto iter = coms.begin(); iter != coms.end(); ++iter) {//for each server
        Com* com_s = iter->get();
        std::string comsip(com_s->ip);
        std::string comsport(com_s->port);
        if(com_s->id%2!=0){//server
            for (auto iter = coms.begin(); iter != coms.end(); ++iter) {
                Com* com = iter->get();
                if(com->id%2==0){//worker
                    std::string comip(com->ip);
                    std::string comport(com->port);
                    //to server
                    packs *p_s=new packs();
                    p_s->meta=Option::SERVER_WORKER_CONNECTED;
                    p_s->msg+=comip+" "+comport+" "+std::to_string(com_s->id)+" "+std::to_string(com->id);
                    p_s->recv_id=com_s->id;
                    p_s->send_id=id;
                    //to worker
                    packs *p=new packs();
                    p->meta=Option::SERVER_WORKER_CONNECTED;
                    p->msg+=comsip+" "+comsport+" "+std::to_string(com_s->id)+" "+std::to_string(com->id);
                    p->recv_id=com->id;
                    p->send_id=id;
                    send_mtx.lock(); // lock
                    send_queue.push(p);
                    send_queue.push(p_s);
                    send_mtx.unlock(); // unlock
                }
            }
        }
    }
}

void PostOffice::schedulerConfirmID(){
    bool flag=true;
    sleep(1);
    while(flag){
        flag=false;
        for (auto iter = coms.begin(); iter != coms.end(); ++iter) {
            Com* com = iter->get();
            if(!com->idSetted){
                flag=true;
            }
        }
    }
}

void PostOffice::server_workerWaitID(){
    packs *p=new packs();
    p->meta=Option::CONFIRM_ID;
    p->recv_id=0;
    p->send_id=id;
    send_mtx.lock(); // lock
    send_queue.push(p);
    send_mtx.unlock(); // unlock
}

void PostOffice::schedulerConfirmConnection(){
    bool flag=true;
    sleep(2);
    // std::cout<<"sw_connected.size()"<<sw_connected.size()<<std::endl;
    while(flag){
        flag=false;
        for (auto it = sw_connected.begin(); it != sw_connected.end(); ++it) {
            auto value = it->second;
            if(value==0){
                flag=true;
            }
        }
    }
}

void PostOffice::serverInformConnect(std::string s_id,std::string w_id){
    packs *p=new packs();
    p->meta=Option::CONFIRM_CONNECTED;
    p->recv_id=0;
    p->send_id=id;
    p->msg+=s_id+" "+w_id;
    send_mtx.lock(); // lock
    send_queue.push(p);
    send_mtx.unlock(); // unlock
}
