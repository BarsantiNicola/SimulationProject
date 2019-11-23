#ifndef __MYTICTOC_TCX_H
#define __MYTICTOC_TCX_H

#include <omnetpp.h>
using namespace omnetpp;

namespace mytictoc {

    class Txc : public cSimpleModule{
        private:
            int count;
            simsignal_t countSignal;
            simsignal_t interProcTimeSignal;
            double interProcTime;

        protected:
            virtual void initialize();
            virtual void handleMessage(cMessage *msg);
    };

}; // namespace

#endif
