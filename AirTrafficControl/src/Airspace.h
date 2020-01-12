#ifndef __CONTROLTOWER_AIRSPACE_H
#define __CONTROLTOWER_AIRSPACE_H

#include <omnetpp.h>

class ControlTower;               //Forward declaration of the ControlTower class

#define TOTAL_AIRPLANES 1000000   //Total number of airplanes that will arrive in the system (= sample size)

using namespace omnetpp;

namespace airport
{
  class Airspace : public cSimpleModule
   {
    private:

      /*--------------Input Parameters--------------*/
     bool isInterArrivalTimeRandom;
     bool isLandingTimeRandom;
     double interArrivalTime;
     double landingTime;

     /*--------Statistics-related Signals----------*/
     simsignal_t holdingQueueSize;
     simsignal_t holdingQueueWaitingTime;
     simsignal_t airportResponseTime;

     /*------------------Other--------------------*/
     unsigned long int departedPlanes; //Number of airplanes that departed from airport (used to stop the simulation after TOTAL_AIRPLANES samples have been collected)
     double nextArrival;               //Arrival time of the next airplane
     double nextLandingTime;           //Landing time of the next airplane
     cQueue* holdingQueue;             //FIFO queue of airplanes waiting for landing
     ControlTower* controlTower;       //Used for cross-module call purposes

    protected:
     virtual void initialize();
     virtual void refreshDisplay() const;
     virtual void handleMessage(cMessage* airplane);
     virtual ~Airspace();

    public:
     virtual double getMaxQueueTime(); //Returns the time the oldest plane entered the holding queue, or "-1.0" if the queue is empty (called by the ControlTower module)
     virtual void go();                //Starts the landing of the oldest plane in the holding queue (called by the ControlTower module)
  };

}; //namespace

#endif
