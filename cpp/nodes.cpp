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
    std::istringstream sin;
    std::string line,fp,sp,tp;
    std::pair<std::string ,std::string> ip;
    // 随机数生成器
    std::default_random_engine generator;
    // 定义分布区间
    std::uniform_int_distribution<int> distribution(0, 4096);
    int node_id=distribution(generator);
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
