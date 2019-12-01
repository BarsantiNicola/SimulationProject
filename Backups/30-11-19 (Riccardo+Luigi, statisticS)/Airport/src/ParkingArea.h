#ifndef __CONTROLTOWER_PARKINGAREA_H_
#define __CONTROLTOWER_PARKINGAREA_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace airport {

    class ParkingArea : public omnetpp::cSimpleModule{
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

            simsignal_t checkParkedPlanes;
            simsignal_t checkDepartQueue;
            simsignal_t airplaneTakeoff;

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
