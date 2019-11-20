#ifndef __BAR_CHEF_H_
#define __BAR_CHEF_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace bar {

    class Chef : public cSimpleModule{
     private:
        simsignal_t counter;
        cQueue* requestQueue;
        cMessage* beepMsg;

      protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
    };

} //namespace

#endif
