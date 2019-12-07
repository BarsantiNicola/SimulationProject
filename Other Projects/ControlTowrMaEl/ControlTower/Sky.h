
#ifndef __CONTROLTOWER_SKY_H_
#define __CONTROLTOWER_SKY_H_
#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

class Sky : public cSimpleModule{

    private:
        simsignal_t numAirplanesStat;                           // Airplanes number statistic
        simsignal_t waitingTimeLanding;                         // Airplanes' waiting time statistic

    protected:
        bool usingRunway = false;                               // Keeps track if there's a landing going on
        bool runwayAvailable= true;                             // Keeps track if there's a pending runway request
        cMessage* nextL;                                        // Keeps track of the next landing event
        SimTime nextLtime;                                      // Keeps track of the time the landing was scheduled at
        cQueue airplanes;                                       // Airplanes queued for landing
        std::vector<double> times;                              // Vector for waiting times
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void scheduleArrival();
        virtual void handleArrival();
        virtual void scheduleLanding();
        virtual void handleLanding();
        virtual void handleStopFromTakeoff();
        virtual void handleAvailableFromTakeoff();

};

#endif
