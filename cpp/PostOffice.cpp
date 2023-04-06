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
    printf("coms size is:%d\n",coms.size());
    while(true){
        struct sockaddr_in recv_addr;
        socklen_t len=sizeof(recv_addr);
        SOCKET receive;
        // std::cout<<"begin accept"<<std::endl;
        receive=accept4(Com::listenSock,(sockaddr*)&recv_addr, &len,SOCK_NONBLOCK);//设置为非阻塞
        // std::cout<<"accept success! receive的sock是: "<<receive<<std::endl;
        //get receive ip

        char *tmpIp=inet_ntoa(recv_addr.sin_addr);

        std::cout<<"ip is :"<<tmpIp<<std::endl;

        for(int i=0;i<coms.size();++i){
            if(!coms[i]->allocated){
                if(!strcmp(tmpIp,coms[i]->ip)){
                    std::cout<<"found!"<<std::endl;
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
        std::cout<<Com::listenSock<<std::endl;
        std::cout<<receive<<std::endl;
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

    char* tmpIp = new char[ip.length() + 1];
    char* tmpPort = new char[port.length() + 1];
    // strcpy(tmpIp, ip.c_str());

    strcpy(tmpIp, "172.21.194.66");
    strcpy(tmpPort, port.c_str());
    if(id%2==0){//worker
        server_workerConnectScheduler(tmpIp,tmpPort);
    }else{//server
        int _com_last;//记录coms的数量
        coms.push_back(std::make_unique<Com>());
        _com_last=coms.size()-1;
        coms[_com_last]->id=std::stoi(worker_id);
        workers.emplace_back(std::stoi(worker_id));
        coms[_com_last]->setSendAddr(tmpIp,tmpPort);
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
            if(!strcmp(tmpIp,tmpip)){
                coms[_com_last]->recvSock=receive;
                coms[_com_last]->recvAddr=recv_addr;
                coms[_com_last]->allocated=true;
                break;
            }
        }
        serverInformConnect(server_id,worker_id);

        std::cout<<"server_id "<<server_id<<"  worker_id "<<worker_id<<std::endl;
        
    }
}

void PostOffice::sendMessage() {
    printf("Enter send thread!\n");
    while(true){
        send_mtx.lock(); // lock
        if (!send_queue.empty()) { 
            packs * p = send_queue.front(); //get a packet from queue
            send_queue.pop(); 
            send_mtx.unlock(); // unlock
            //packet send message
            const size_t buffer_size = sizeof(Option) + 4 * sizeof(int) + p->weights.size() * sizeof(int) + p->msg.length() + 1;//'\n'
            // std::cout<<"p->msg.length"<<p->msg.length()<<std::endl;
            char* buffer = new char[buffer_size];
            serialize(*p, buffer, buffer_size);//Byte serialization 

            // std::string s;
            // s.append(reinterpret_cast<const char*>(&p->meta), sizeof(p->meta));
            // s.append(reinterpret_cast<const char*>(&p->send_id), sizeof(p->send_id));
            // s.append(reinterpret_cast<const char*>(&p->recv_id), sizeof(p->recv_id));
            // for (int i = 0; i < p->weights.size(); i++) {
            //     s.append(reinterpret_cast<const char*>(&p->weights[i]), sizeof(int));
            // }
            // s.append(p->msg);
            // s += DELIMITER;

            for (auto iter = coms.begin(); iter != coms.end(); ++iter) {
                Com* com = iter->get();
                if(com->id!=p->recv_id) continue;
                else{
                    //send
                    //std::cout<<"等待发送"<<std::endl;
                    while(!send(com->sendSock, buffer, buffer_size, 0));
                    //std::cout << "Send a packet: " << buffer_size << " bytes" << std::endl;
                    break;
                }
            }
            delete[] p->weights.data();
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

    //std::cout<<"coms.size(): "<<coms.size()<<std::endl;

    int timeout=500;//ms

    while(true){
        int ret = poll(pollFdVec.data(), pollFdVec.size(), timeout);//moniter the recv message
        if (ret < 0) {
            // printf("poll函数出错！\n");
            return ;
        } else if (ret == 0) {
            // printf("没有接收到\n");
            continue;
        } else {
            for (size_t i = 0; i < pollFdVec.size(); i++) {
                if (pollFdVec[i].revents & POLLIN) {//找到是哪个发送了
                    //std::cout<<" 第 "<< i <<"个接收到了信号！"<<std::endl;
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
                                std::cout<<"RIGHT"<<std::endl;
                            }
                            else{
                                std::cout<<"WRONG"<<std::endl;
                            }
                            break;
                        default:
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
        std::cout<<"id: "<<com->id<<"   ip: " <<com->ip<<"    port: "<<com->port<<"sendSock="<<com->sendSock<<" recvSock="<<com->recvSock<<std::endl;
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
    std::cout<<"sw_connected.size()"<<sw_connected.size()<<std::endl;
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
