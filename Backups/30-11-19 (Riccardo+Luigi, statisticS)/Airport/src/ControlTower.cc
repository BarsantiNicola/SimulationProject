#include "Airplane_m.h"
#include "ControlTower.h"

namespace airport {

    Define_Module(ControlTower);
    void ControlTower::initialize(int stage){
        if(stage == 1){
            try{
                landingStripOccupied = false;
                airspace = check_and_cast<Airspace*>(getModuleByPath("Airport.airspace"));
                parkingArea = check_and_cast<ParkingArea*>(getModuleByPath("Airport.aerodrome.parkingarea"));
                EV<<"[CONTROL TOWER] Initialized"<<endl;
            }catch(const cRuntimeError& e){
                EV<<"check_and_cast() error"<<endl;
                EV<<e.what()<<endl;
            }
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

            double airSpaceQueueTime = airspace->getMaxQueueTime();
            double parkingAreaQueueTime = parkingArea->getMaxQueueTime();

            if(airSpaceQueueTime < 0 && parkingAreaQueueTime < 0)
                landingStripOccupied = false;
            else
                if(airSpaceQueueTime < 0)
                    parkingArea->pop();
                else
                    if(parkingAreaQueueTime < 0)
                        airspace->pop();
                    else
                        if(airSpaceQueueTime < parkingAreaQueueTime)
                         airspace->pop();
                        else
                            parkingArea->pop();
            EV<<"completed() has been called"<<endl;

    }
}; //namespace
