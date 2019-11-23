#ifndef HUB_H_
#define HUB_H_

#include <omnetpp.h>
using namespace omnetpp;

namespace mytictoc {

    class Hub : public cSimpleModule{
        private:
            int n;
            cPar *port;

        protected:
            virtual void initialize();
            virtual void handleMessage(cMessage *msg);
    };
}

#endif
