
#ifndef __CONTROLTOWER_TAKEOFF_H_
#define __CONTROLTOWER_TAKEOFF_H_

#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

class TakeOff : public cSimpleModule{

    private:
        simsignal_t numAirplanesStat;                               // Statistic for number of airplanes
        simsignal_t waitingTimeTakeOff;                             // Statistic for airplanes' waiting time

    protected:
        bool runwayAvailable = false;                               // Keeps track if runway is available
        cQueue airplanes;                                           // Queue of airplanes for takeoff
        std::vector<double> times;                                  // Vector for waiting times
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void handleTakeOff();
        virtual void scheduleTakeOff();
        virtual void handleOkFromSky();
        virtual void handleAirplaneFromParking(cMessage* msg);

};

#endif
