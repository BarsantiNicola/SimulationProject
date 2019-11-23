#include "Consumer.h"

namespace producerconsumer {

    Define_Module(Consumer);

    void Consumer::initialize(int stages){
        if(stages == 1){
            EV<<"Initialize Consumer...";
            consumptionTime = par("consumptionTime").doubleValue();
            beepMsg = new cMessage("beep");
            scheduleAt(simTime(), beepMsg);
            id = par("id").intValue();
            pendentRequest = 0;
        }
    }

    void Consumer::handleMessage(cMessage *msg){
        if (msg->isSelfMessage()){
            send(new cMessage("RequestMessage", id-1), "out");
            pendentRequest++;
            EV<<getName()<<": sent message to deliverer (#2";
            scheduleAt(simTime() + consumptionTime, msg);
        }else{
            EV<<getName()<<": pendent requests "<<--pendentRequest;
        }
    }
    void Consumer::finish(){
        cancelAndDelete(beepMsg);
    }
};
