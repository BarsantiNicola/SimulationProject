#ifndef __CONTROLTOWER_CONTROLTOWER_H_
#define __CONTROLTOWER_CONTROLTOWER_H_

#include <omnetpp.h>
#include "Airplane_m.h"
#include "Airspace.h"
#include "ParkingArea.h"

using namespace omnetpp;

namespace airport {

    class ControlTower : public cSimpleModule{
        private:
            bool landingStripOccupied;
            cModule* airspace;
            cModule* parkingArea;

        protected:
            virtual void initialize(int stage);
            virtual int numInitStages() const{ return 2;}
        public:
            virtual bool notify();
            virtual void completed();
    };
}; //namespace

#endif
