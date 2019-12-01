#ifndef __CONTROLTOWER_TCX_H
#define __CONTROLTOWER_TCX_H

#include <omnetpp.h>
#include "Airplane_m.h"
#include "ControlTower.h"

using namespace omnetpp;

namespace airport{

    class Airspace : public cSimpleModule{
        private:
            bool isArrivalRateRandom;
            double interArrivalRate;
            double nextArrival;
            bool isLandingTimeRandom;
            double landingTimeRate;
            double landingTime;
            cQueue* holdingQueue;
            cModule* controlTower;

        protected:
            virtual void initialize();
            virtual void handleMessage(cMessage* airplane);
            virtual void finish();

        public:
            virtual double getMaxQueueTime();
            virtual void pop();
    };
}; //namespace
#endif
