//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_SERVER_H
#define DIS_SERVER_H
#include <string>
#include "nodes.h"

class server :public nodes{
private:
    bool begin;
    float alpha;
    std::vector<float> w;


public:
    server(){};
    void init();
    void waitBegin() const;
    void maintainWeights(std::vector<float> & weights);
    void dealWithPush(std::vector<float> & weights);
    void dealWithPull(int sendId);

};


#endif //DIS_SERVER_H
