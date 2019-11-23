#include "Waitress.h"
#include "Order_m.h"

namespace bar {

    Define_Module(Waitress);

    void Waitress::initialize(){
        scheduleAt(simTime() + uniform(0, 3) , new cMessage());
    }

    void Waitress::handleMessage(cMessage *beepMsg){
        scheduleAt(simTime() + uniform(0, 3), new cMessage());
        Order* order = new Order();
        order->setIndex(uniform(0, 3));
        send(order, "out");
    }
};
