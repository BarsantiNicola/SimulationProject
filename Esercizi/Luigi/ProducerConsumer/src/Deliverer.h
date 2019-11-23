#ifndef __PRODUCERCONSUMER_DELIVERER_H_
#define __PRODUCERCONSUMER_DELIVERER_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace producerconsumer {

    class Deliverer : public cSimpleModule {
        private:
            int n;
            bool* freeProducers;
            int firstFreeProducer;
            cQueue* requestQueue;

        protected:
            virtual void initialize();
            virtual void handleMessage(cMessage *msg);
            virtual void finish();
    };
};

#endif
