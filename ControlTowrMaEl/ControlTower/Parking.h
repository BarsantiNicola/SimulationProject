
#ifndef __CONTROLTOWER_PARKING_H_
#define __CONTROLTOWER_PARKING_H_

#include <omnetpp.h>

using namespace omnetpp;

class Parking : public cSimpleModule{

    private:
        simsignal_t numAirplanesStat;                   // Statistic for airplanes number

    protected:
        int numAirplanes = 0;                           // Number of parked airplanes
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

};

#endif
