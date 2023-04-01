//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_DATA_H
#define DIS_DATA_H

#include <fstream>
#include <sstream>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

class scheduler;

class frind;

class Data {
private:
    char *para_path=new char(100);
    std::string config_path;
    std::string data_path;
    std::string role;
    std::string ip;
    std::string port;
    std::vector<std::vector<float>> data;
    std::unordered_map<std::string, std::string> para;
    std::unordered_map<int,std::pair<std::string,std::string>> nodes;
    bool normalization;
    bool regularization;
    float regular_para;
    int epoch;
    float alpha;

public:
    Data(){};
    void init();  //1.load parameters(para.txt) 2.set parameters 3.load training/test data
    void loadParameters();  //load parameters from parameter file
    void loadData();  //load data from data file
    void setParaPath(char *_para_path);  //set original paraPath **
    std::string getRole();  //get role
    ~Data();
    friend class nodes;
    friend class scheduler;
    friend class server;
    friend class worker;
    
};


#endif //DIS_DATA_H
