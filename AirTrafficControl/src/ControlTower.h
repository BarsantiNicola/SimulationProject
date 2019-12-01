#ifndef __CONTROLTOWER_CONTROLTOWER_H_
#define __CONTROLTOWER_CONTROLTOWER_H_

#include <omnetpp.h>
#include "Airspace.h"
#include "ParkingArea.h"

using namespace omnetpp;

namespace airport
{
  class ControlTower : public cSimpleModule
   {
    private:
     bool landingStripOccupied;              //Whether the landing strip is currently occupied by a takingoff or landing airplane
     Airspace* airspace;                     //Used for cross-module call purposes
     ParkingArea* parkingArea;               //Used for cross-module call purposes

    protected:
     virtual void initialize(int stage);
     virtual int numInitStages() const{ return 2;}

    public:
     virtual bool notify();                  //Notification that an airplane arrived in the holding or the departing queue, to which the Control Tower responds
                                             //on whether such plane can immediately start its takeoff or landing (called by the Airspace and ParkingArea modules)
     virtual void completed();               //Notification that an airplane completed its takeoff or landing, to which the Control Tower determines the next
                                             //airplane allowed to takeoff or landing, if any (called by the Airspace and ParkingArea modules)
   };

}; //namespace

#endif
