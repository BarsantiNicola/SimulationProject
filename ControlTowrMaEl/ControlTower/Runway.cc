
#include "Runway.h"
#include <string.h>
Define_Module(Runway);

void Runway::initialize(){

}

void Runway::handleMessage(cMessage *msg){

    if(strcmp(msg->getArrivalGate()->getFullName(), "sky") == 0){       // If airplane is from sky
        send(msg, "park");                                      // forward it to the park
    }

}
