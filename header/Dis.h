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
    s->post_office->showComsMessage();
    s->beginWorking();
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

    s->post_office->showComsMessage();

    std::cout<<"开始训练"<<std::endl;

    s->maintainWeights();

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

    s->post_office->showComsMessage();

    std::cout<<"开始训练"<<std::endl;

    int epoch=300;
    sleep(3);
    auto start = std::chrono::high_resolution_clock::now();

    while(epoch--){
        // sleep(0.8); 
        std::cout<<300-epoch<<std::endl;
        // std::cout<<"第 " <<10000-epoch <<" 次迭代"<<std::endl;
        // pull weights
        // std::cout<<"拉取参数"<<std::endl;
        s->pull(s->post_office->servers[0]);
        // for(int i=0;i<s->w.size();i++){
        //     std::cout<<s->w[i]<<" ";
        // }
        // std::cout<<std::endl;
        // caculate
        // std::cout<<"正在训练......"<<std::endl;

        std::vector<std::vector<float>> r_samples;
        std::vector<float> res;
        s->random_sample(s->datas->data,r_samples,500000);//get samples
        res=s->lr->train(s->w,r_samples);
        s->w=res;
        // std::cout<<"训练后的参数是："<<std::endl;
        // for(int i=0;i<s->w.size();i++){
        //     std::cout<<s->w[i]<<" ";
        // }
        // std::cout<<std::endl;

        // push res
        // std::cout<<"推送参数"<<std::endl;


        s->push(s->post_office->servers[0]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // 将时间差转换为秒
    double seconds = duration.count() * 1.0 / 1000000;

    // 输出结果
    std::cout << "程序运行时间：" << seconds << " 秒" << std::endl;

    s->pull(s->post_office->servers[0]);

    std::cout<<"训练后的参数是："<<std::endl;
    for(int i=0;i<s->w.size();i++){
        std::cout<<s->w[i]<<" ";
    }
    std::cout<<std::endl;


    getchar();
    getchar();
}

#endif //DIS_DIS_H
