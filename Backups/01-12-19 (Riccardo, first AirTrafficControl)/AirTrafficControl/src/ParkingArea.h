#ifndef __CONTROLTOWER_PARKINGAREA_H_
#define __CONTROLTOWER_PARKINGAREA_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace airport
{

 class ParkingArea : public omnetpp::cSimpleModule
  {
   private:

    /*--------------Input Parameters--------------*/
    bool isParkingTimeRandom;
    bool isTakeoffTimeRandom;
    double parkingTime;
    double takeoffTime;

    /*--------Statistics-related Signals----------*/
    simsignal_t parkedPlanes;
    simsignal_t departQueueSize;
    simsignal_t departQueueWaitingTime;

    /*------------------Other--------------------*/
    long numParked;             //Number of airplanes currently parked
    double nextParkingTime;     //Parking time of the next airplane
    double nextTakeoffTime;     //Takeoff time of the next airplane
    cQueue* departQueue;        //FIFO queue of airplanes waiting for takeoff
    cModule* controlTower;      //Used for cross-module call purposes

   protected:
    virtual void initialize();
    virtual void refreshDisplay() const;
    virtual void handleMessage(cMessage* airplane);
    virtual ~ParkingArea();

   public:
    virtual double getMaxQueueTime();  //Returns the time the oldest plane entered the departing queue, or "-1" if the queue is empty (called by the ControlTower module)
    virtual void go();                 //Starts the takeoff of the oldest plane in the departing queue (called by the ControlTower module)
  };

}; //namespace

#endif
