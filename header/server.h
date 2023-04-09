//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_SERVER_H
#define DIS_SERVER_H
#include <string>
#include "nodes.h"

class server :public nodes{
private:
    std::vector<float> w;


public:
    server(){};
    void init();
    void setAlpha(float a);
    void waitBegin() const;
    void maintainWeights();
    void dealWithPush(std::vector<float> & weights);
    void dealWithPull(int sendId);
};


#endif //DIS_SERVER_H
