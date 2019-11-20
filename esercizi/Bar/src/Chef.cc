#include "Chef.h"
#include "Order_m.h"
namespace bar {

    Define_Module(Chef);

    void Chef::initialize(){
        counter = registerSignal("order");
        requestQueue = new cQueue();
        beepMsg = new cMessage();
    }

    void Chef::handleMessage(cMessage *msg){
        Order *order;
        if(msg->isSelfMessage()){
            requestQueue->pop();
            emit(counter, 1);
            if(not requestQueue->isEmpty()){
                order = (Order*)requestQueue->get(0);
                scheduleAt(simTime() + order->getIndex(), beepMsg);
            }
        }else{
            order = (Order*)msg;
            bool isEmpty = requestQueue->isEmpty();
            requestQueue->insert(msg);
            EV<<"1"<<endl;
            if(isEmpty){
                scheduleAt(simTime() + order->getIndex(), beepMsg);
                EV<<"2"<<endl;
            }
        }
    }
    void Chef::finish(){
        requestQueue->clear();
        delete requestQueue;
        cancelAndDelete(beepMsg);
    }
} //namespace
