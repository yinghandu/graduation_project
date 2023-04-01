//
// Created by duyin on 2023/2/7.
//

#include "../header/nodes.h"

void nodes::setData(Data *d) {
    if (data != nullptr) {
        delete data; // 如果 data 已经指向某个内存块，先释放它
    }
    data = new Data(*d); // 分配一个新的内存块，并将 d 所指向的对象拷贝到其中
}

void nodes::loadConfig(const std::string &configPath) {
    std::ifstream inFile(configPath, std::ios::in);
    if (!inFile.is_open()) {
        printf("Error:打开config文件失败!\n");
    } 
    std::istringstream sin;
    std::string line,fp,sp,tp;
    std::pair<std::string ,std::string> ip;
    // 随机数生成器
    srand(time(nullptr));
    // 定义分布区间
    int node_id=rand() % 99 + 1;;
    while (std::getline(inFile, line)){
        sin.clear();
        sin.str(line);
        sin>>fp;
        sin>>sp;
        sin>>tp;
        ip.first=sp;
        ip.second=tp;
        if(fp=="worker") node_id*=2;
        if(fp=="server") node_id=node_id*2+1;
        if(fp=="scheduler") node_id=0;
        __nodes.emplace(node_id,ip);
        std::cout <<"node_id: "<<node_id<< ", IP地址：" << ip.first << "，端口号：" << ip.second << std::endl;
    }
}

void nodes::send_test() {
    for(int i=0;i<10;i++){
        for(int j=0;j<post_office->coms.size();j++){    
            packs *p=new packs();
            p->msg.reserve(100); 
            p->msg+=std::to_string(i);
            p->recv_id=post_office->coms[j]->id;
            send_mtx.lock(); // lock
            send_queue.push(p);
            send_mtx.unlock(); // unlock
        }
    }
    std::cout<<"send_test"<<std::endl;
    std::cout<<"队列长度:"<<send_queue.size()<<std::endl;
}

void nodes::recv_test() {
    while(true){
        recv_mtx.lock(); // lock
        if (!recv_queue.empty()) { 
            packs * p = recv_queue.front(); //get a packet from queue
            recv_queue.pop();
            
            std::cout<<"接收的信息包括："<<p->msg<<std::endl;

            std::cout<<"接收的id="<<p->send_id<<std::endl;
            std::cout<<"目前所剩队列长度："<<recv_queue.size()<<std::endl;
            recv_mtx.unlock(); // unlock
        } else {
            recv_mtx.unlock(); // unlock
            continue;
        }
    }
}
