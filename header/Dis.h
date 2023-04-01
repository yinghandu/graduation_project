//
// Created by duyin on 2023/2/7.
//

#ifndef DIS_DIS_H
#define DIS_DIS_H

#include "Data.h"
#include "scheduler.h"
#include "server.h"
#include "worker.h"

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
    //s->send_test();
    //s->recv_test();
    s->post_office->showComsMessage();
    getchar();
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
    //s->send_test();
    //s->recv_test();
    getchar();
}

void Worker(Data *d){
    printf("启动worker结点\n");
    worker *s=new worker() ;
    if (d == nullptr) {
        printf("错误！data指针为空!");
        return;
    }
    s->setData(d);  //set loaded data
    delete d;
    s->init();  //init the server according to the parameters given by data->config_path
    //s->send_test();
    //s->recv_test();
    getchar();
}

#endif //DIS_DIS_H
