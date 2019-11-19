#include "Deliverer.h"

namespace producerconsumer {

    Define_Module(Deliverer);

    void Deliverer::initialize(){
        EV<<"Initialize Deliverer...";
        n = par("nProducers").intValue();
        freeProducers = new bool[n];
        for(int i = 0; i < n; i++)
            freeProducers[i] = true;
        requestQueue = new cQueue();
        firstFreeProducer = 0;
    }

    void Deliverer::handleMessage(cMessage *msg){
        if (strcmp(msg->getArrivalGate()->getName(), "freeNotification") == 0){
            freeProducers[msg->getArrivalGate()->getIndex()] = true;
            if(firstFreeProducer == -1){
                firstFreeProducer = msg->getArrivalGate()->getIndex();
            }
            if(!requestQueue->isEmpty()){
                send((cMessage*)requestQueue->pop(), "out", firstFreeProducer);
                freeProducers[firstFreeProducer] = false;
                firstFreeProducer = freeProducers[(firstFreeProducer + 1) % 2]?((firstFreeProducer + 1) % 2):-1;
            }
        }else{ // If a request from a consumer arrives
            if(firstFreeProducer != -1){

                send(msg, "out", firstFreeProducer);
                freeProducers[firstFreeProducer] = false;
                firstFreeProducer = freeProducers[(firstFreeProducer + 1) % 2]?((firstFreeProducer + 1) % 2):-1;
            }else{
                requestQueue->insert(msg);
            }
        }
    }

    void Deliverer::finish(){
        requestQueue->clear();
        delete requestQueue;
        delete freeProducers;
    }
};
