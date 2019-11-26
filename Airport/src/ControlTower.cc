#include "Airplane_m.h"
#include "ControlTower.h"
#include "Airspace.h"
#include "ParkingArea.h"

namespace airport {

    Define_Module(ControlTower);
    void ControlTower::initialize(int stage){
        if(stage == 1){
            landingStripOccupied = false;
            airspace = getModuleByPath("Airport.airspace");
            parkingArea = getModuleByPath("Airport.aerodrome.parkingarea");
            EV<<"[CONTROL TOWER] Initialized"<<endl;
        }
    }

    bool ControlTower::notify(){
        Enter_Method("notify()");
        EV<<"Someone called me"<<endl;
        if(!landingStripOccupied){
            landingStripOccupied = true;
            return true;
        }
        return false;
    }

    void ControlTower::completed(){
        Enter_Method("completed()");
        double airSpaceQueueTime = (check_and_cast<Airspace*>(airspace))->getMaxQueueTime();
        double parkingAreaQueueTime = (check_and_cast<ParkingArea*>(parkingArea))->getMaxQueueTime();

        if(airSpaceQueueTime < 0 && parkingAreaQueueTime < 0)
            landingStripOccupied = false;
        else
            if(airSpaceQueueTime < 0)
                check_and_cast<ParkingArea*>(parkingArea)->pop();
            else
                if(parkingAreaQueueTime < 0)
                    check_and_cast<Airspace*>(airspace)->pop();
                else
                    if(airSpaceQueueTime < parkingAreaQueueTime)
                        check_and_cast<Airspace*>(airspace)->pop();
                    else
                        check_and_cast<ParkingArea*>(parkingArea)->pop();

        EV<<"completed() has been called"<<endl;
    }
}; //namespace
