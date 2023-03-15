//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_DIS_H
#define DIS_DIS_H

#include "Data.h"
#include "scheduler.h"



void Scheduler(Data *d){
    printf("启动schduler结点\n");
    scheduler *s=new scheduler() ;
    if (d == nullptr) {
        printf("错误！data指针为空!");
        return;
    }
    s->setData(d);  //set loaded data
    delete d;
    s->init();  //init the scheduler according to the parameters given by data->config_path
}

void Server(Data *d){
    printf("启动server结点\n");
    server *s=new server() ;
    if (d == nullptr) {
        printf("错误！data指针为空!");
        return;
    }
    s->setData(d);  //set loaded data
    delete d;
    s->init();  //init the server according to the parameters given by data->config_path
}

#endif //DIS_DIS_H
