#include "Owner.h"
#include "Order_m.h"

namespace bar {

    Define_Module(Owner);

    void Owner::initialize(){
        T = getParentModule()->par("T").doubleValue();
        interArrivalTime = registerSignal("interArrivalTime");
        lastOrderTime = simTime().dbl();
    }

    void Owner::handleMessage(cMessage *msg){
        emit(interArrivalTime, simTime().dbl()-lastOrderTime);
        lastOrderTime = simTime().dbl();
        Order* order = (Order *)msg;
        if(order->getIndex() > T)
            send(msg,"out",1);
        else
            send(msg,"out",0);
    }

} //namespace
