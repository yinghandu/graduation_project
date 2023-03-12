#include <iostream>
#include "header/Dis.h"

int main(int argc,char **argv) {
    Data *dtmp=new Data();  //creat tmp Data
    dtmp->setParaPath(argv[1]);  //set parameter file path pata.txt
    
    dtmp->init();  //init()
    if(dtmp->getRole()=="scheduler")
        Scheduler(dtmp);//if role is scheduler ,carry out scheduler function

    return 0;
}



