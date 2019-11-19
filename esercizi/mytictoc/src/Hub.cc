#include "Hub.h"

namespace mytictoc {

    Define_Module(Hub);

    void Hub::initialize(){
        n = rand()%par("n").intValue();
        port = &par("port");
    }

    void Hub::handleMessage(cMessage *msg){
        if (par("randHub").boolValue()){
            //port = intuniform(0, n); //uniform(0,5);
            int randomPort = port->intValue();
            EV << getName() << " randomPort " << randomPort;
            send(msg, "out", randomPort);
        }else{
            EV<<"Msg arrived on gate "<<msg->getArrivalGate()->getName()<<" (PORT: "<<msg->getArrivalGate()->getIndex()<<")";
            int fixedPort = (msg->getArrivalGate()->getIndex() + 1)%n;
            send(msg, "out", fixedPort);
        }
    }
}; // namespace
