#ifndef __BAR_OWNER_H_
#define __BAR_OWNER_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace bar {

    class Owner : public cSimpleModule{
    private:
        double lastOrderTime, T;
        simsignal_t interArrivalTime;

      protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
    };

} //namespace

#endif
