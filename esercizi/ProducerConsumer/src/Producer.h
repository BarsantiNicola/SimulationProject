#ifndef PRODUCER_H_
#define PRODUCER_H_

#include <omnetpp.h>
using namespace omnetpp;

namespace producerconsumer {

    class Producer : public cSimpleModule {
        private:
            double responseTime;

        protected:
            virtual void initialize();
            virtual void handleMessage(cMessage *msg);
    };
};

#endif
