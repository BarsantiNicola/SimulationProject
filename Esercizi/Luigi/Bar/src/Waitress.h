#ifndef __BAR_WAITRESS_H
#define __BAR_WAITRESS_H

#include <omnetpp.h>

using namespace omnetpp;

namespace bar {

    class Waitress : public cSimpleModule{

        protected:
            virtual void initialize();
            virtual void handleMessage(cMessage *msg);
    };

}; // namespace

#endif
