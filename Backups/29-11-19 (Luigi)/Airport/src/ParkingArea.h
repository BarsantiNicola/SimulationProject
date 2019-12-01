#ifndef __CONTROLTOWER_PARKINGAREA_H_
#define __CONTROLTOWER_PARKINGAREA_H_

#include <omnetpp.h>
#include "Airplane_m.h"
#include "ControlTower.h"

using namespace omnetpp;

namespace airport {

    class ParkingArea : public cSimpleModule{
        private:
            bool isParkingTimeRandom;
            double parkingTimeRate;
            bool isTakeoffTimeRandom;
            double takeoffTimeRate;
            cQueue* departQueue;
            double parkingTime;
            double takeoffTime;
            long numParked;
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
