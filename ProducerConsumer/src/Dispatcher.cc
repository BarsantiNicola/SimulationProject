#include "Dispatcher.h"

namespace producerconsumer {

    Define_Module(Dispatcher);

    void Dispatcher::handleMessage(cMessage *msg){
        EV<<"Received Message from Consumer "<<msg->getArrivalGate()->getIndex()<<". Now it is delivered to Consumer "<<msg->getKind();
        send(msg, "out", msg->getKind());
    }
};
