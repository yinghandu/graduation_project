//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_WORKER_H
#define DIS_WORKER_H
#include <string>
#include "nodes.h"

class worker :public nodes {
private:
    bool begin;
    std::vector<float> w;

public:
    worker(){};
    void init();
    void waitBegin() const;
    void random_sample(const std::vector<std::vector<float>>& data,std::vector<std::vector<float>>& samples ,int num_samples);
    void pull(int ID);
    void push(int ID);
};


#endif //DIS_WORKER_H
