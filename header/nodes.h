//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_NODES_H
#define DIS_NODES_H
#include "Com.h"
#include "Data.h"
#include "PostOffice.h"


class nodes {
protected:
    PostOffice *post_office=new PostOffice();
    std::unordered_map<int,std::pair<std::string,std::string>> __nodes;//node特有数据结构，保存与之联系的结点类型及ip
    Data *data;

public:
    void setData(Data *d);
    void loadConfig(const std::string &configPath);
    virtual void init()=0;
};


#endif //DIS_NODES_H
