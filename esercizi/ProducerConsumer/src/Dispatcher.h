#ifndef __PRODUCERCONSUMER_DISPATCHER_H_
#define __PRODUCERCONSUMER_DISPATCHER_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace producerconsumer {

    class Dispatcher : public cSimpleModule {
        protected:
            virtual void handleMessage(cMessage *msg);
    };
};

#endif
