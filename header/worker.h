//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_WORKER_H
#define DIS_WORKER_H
#include <string>
#include "nodes.h"
#include "LogisticRegression.h"

class worker :public nodes {
private:
   

public:
    worker(){};
    void init();
    void waitBegin() const;
    void random_sample(const std::vector<std::vector<float>>& data,std::vector<std::vector<float>>& samples ,int num_samples);
    void pull(int ID);
    void push(int ID);
    
    
    void serverKeepAlive();

    std::vector<float> w;

    Logistic * lr;
};


#endif //DIS_WORKER_H
