//
// Created by duyin on 2023/2/7.
//


#include "../header/Data.h"

void Data::loadParameters() {
    std::ifstream inFile(para_path, std::ios::in);
    std::istringstream sin;
    std::string line,fp,sp;
    if (errno) {
        std::cerr << "Error opening file1: " << para_path << " (" << strerror(errno) << ")" << std::endl;
        exit(0);
    }
    else{
        printf("打开文件成功!\n");
    }
    while (std::getline(inFile, line)){
        sin.clear();
        sin.str(line);
        sin>>fp;
        sin>>sp;
        para.emplace(std::pair<std::string,std::string>(fp,sp));
    }
    printf("读入成功!\n ");
    
}

void Data::loadData() {
    std::cout<<"开始加载数据\n"<<std::endl;

    if(data_path==" "){
        std::cout<<"训练数据为空\n"<<std::endl;
        return ;
    }
    
    std::ifstream inFile(data_path, std::ios::in);
    std::istringstream sin;         //istringstream
    std::string tmp;
    std::vector<float> rowData;

    if(inFile.fail()){
        std::cerr << "Error opening file2: " << para_path << " (" << strerror(errno) << ")" << std::endl;
        exit(1);
    }
    std::string strLine;
    std::getline(inFile, strLine);//read the first row
    strLine.clear();
    while (std::getline(inFile, strLine))
    {
        sin.clear();
        sin.str(strLine);//Assign a value to the stream
        while (std::getline(sin, tmp, ' '))
        {
            rowData.push_back(stof(tmp)); //push each data
        }
        data.push_back(rowData);
        rowData.clear();
    }

    std::cout<<"加载数据成功! 有 "<<data.size()<<" 行和 "<<data[0].size()<<" 列!"<<std::endl;
}

void Data::setParaPath(char *_para_path) {
    strcpy(para_path,_para_path);
}

void Data::init() {
    printf("开始加载参数\n");
    loadParameters();
    data_path=para.find("data_path")!=para.end()?para.find("data_path")->second:" ";
    config_path=para.find("config_path")->second;
    epoch=(para.find("epoch")!=para.end()&&para.find("epoch")->second!="-1")?std::stoi(para.find("epoch")->second):100;
    alpha=(para.find("alpha")!=para.end()&&para.find("alpha")->second!="-1")?std::stof(para.find("alpha")->second):1e-1;
    regularization=(para.find("regularization")!=para.end())?true:false;
    if(regularization) regular_para=std::stof(para.find("regularization")->second);
    normalization=(para.find("normalization")!=para.end())?true:false;
    role=para.find("role")->second;
    ip=para.find("ip")->second;
    port=para.find("port")->second;
    if(role=="scheduler"||role=="server"){
        data_path.clear();
        data_path+=" ";
        return ;
    } 
    loadData();
}

std::string Data::getRole() {
    return role;
}

Data::~Data() {
    delete[] para_path;
    for (auto& vec : data) {
        vec.clear();
    }
    data.clear();
    para.clear();
    nodes.clear();
}