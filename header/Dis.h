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
    // s->send_test();
    // s->recv_test();
    // s->post_office->showComsMessage();
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

    s->waitBegin();

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
    
    s->waitBegin();

    int epoch=10;

    while(epoch--){
        //pull weights
        std::vector<float> weights;
        std::vector<float> res;
        //
        std::vector<std::vector<float>> r_samples;
        s->random_sample(s->datas->data,r_samples,20);//get samples
        res=s->lr->train(weights,r_samples);
        
        //push res
        //

    }

    getchar();
}

#endif //DIS_DIS_H
