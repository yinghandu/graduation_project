//
// Created by duyin on 2023/4/5.
//

#include "../header/LogisticRegression.h"

std::vector<float> Logistic::train(std::vector<float> &weights, std::vector<std::vector<float>> &data) {
    int col=data[0].size(),row=data.size();//row,col

    int i,j,bias=weights[col-1];

    std::vector<float> res(col);
    float b=0,z;

    for(i=0;i<row;++i){
        z=0;
        for(j=0;j<col-1;++j){//data[col-1] is label
            z+=data[i][j]*weights[j];
        }
        z+=bias;
        z=sigmoid(z);
        for(j=0;j<col-1;++j){
            res[j]+=(z-data[i][col-1])*data[i][j];
        }
        b+=z-data[i][col-1];
    }

    for(j=0;j<col-1;++j){
        res[j]/=row;
    }

    b/=row;

    weights[col-1]=b;

    return weights; // 返回weights的引用
}
