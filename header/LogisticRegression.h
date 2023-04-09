//
// Created by duyin on 2023/4/5.
//

#ifndef DIS_LOGISTICREGRESSION_H
#define DIS_LOGISTICREGRESSION_H

#include "Tools.h"



class Logistic
{
public:
    std::vector<float> train(std::vector<float> &weights,std::vector<std::vector<float>> &data);
    
};

#endif //DIS_LOGISTICREGRESSION_H

