
#ifndef __CONTROLTOWER_RUNWAY_H_
#define __CONTROLTOWER_RUNWAY_H_

#include <omnetpp.h>

using namespace omnetpp;

class Runway : public cSimpleModule{

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

};

#endif
