#include "Producer.h"

namespace producerconsumer {

    Define_Module(Producer);

    void Producer::initialize(){
        EV<<"Initialize Producer...";
        responseTime = par("responseTime").doubleValue();
    }

    void Producer::handleMessage(cMessage *msg){
        if (msg->isSelfMessage()){
            send(msg, "out");
            EV<<getName()<<": job (for consumer "<<msg->getKind()<<") sent to dispatcher";
            send(new cMessage("free"), "outNotification");
        }else{
            scheduleAt(simTime() + responseTime, new cMessage("ResponseMessage", msg->getKind()));
        }
    }
};
