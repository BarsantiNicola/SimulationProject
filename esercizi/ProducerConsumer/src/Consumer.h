#ifndef CONSUMER_H_
#define CONSUMER_H_

#include <omnetpp.h>
using namespace omnetpp;

namespace producerconsumer {

    class Consumer : public cSimpleModule {
        private:
            int pendentRequest, id;
            double consumptionTime;
            cMessage* beepMsg;

        protected:
            virtual void initialize(int stages);
            virtual int numInitStages() const{ return 2;}
            virtual void handleMessage(cMessage *msg);
            virtual void finish();
    };
};

#endif
