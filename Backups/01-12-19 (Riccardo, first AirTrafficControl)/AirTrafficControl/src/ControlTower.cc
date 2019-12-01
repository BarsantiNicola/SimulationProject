#include "Airplane_m.h"
#include "ControlTower.h"                    //TODO: Remove?

namespace airport
{
 Define_Module(ControlTower);

 /* Module initializing function */
 void ControlTower::initialize(int stage)
  {
   if(stage == 1)
    {
     try                                   //This try block was inserted to catch possible cRuntimeError exceptions raised by the check_and_cast<> function (which should never happen)
      {
       landingStripOccupied = false;       //Initialize the landing strip as available
       airspace = check_and_cast<Airspace*>(getModuleByPath("AirTrafficControl.airspace"));                     //Retrieve the address of the airspace module
       parkingArea = check_and_cast<ParkingArea*>(getModuleByPath("AirTrafficControl.airport.parkingarea"));    //Retrieve the address of the parkingarea module
      }
     catch(const cRuntimeError& e)         //Should never happen
      {
       char err[60];

       //Displays an error message (pausing the simulation in the QTENV interface)
       sprintf(err,"WARNING: check_and_cast error, event number: %li", getSimulation()->getEventNumber());
       getSimulation()->getActiveEnvir()->alert(err);

       //Prints the stack trace
       EV<<e.what()<<endl;
      }
    }
  }

 /* Notification that an airplane arrived in the holding or the departing queue (called by the Airspace and ParkingArea modules) */
 bool ControlTower::notify()
  {
   Enter_Method("notify()");          //Denotes that this member function is callable from other modules (in our case, the Airspace and the ParkingArea)
   if(!landingStripOccupied)          //If the airport landing strip is not occupied
    {
     landingStripOccupied = true;     //Set the airport landing strip as occupied
     return true;                     //Inform the caller that the airplane's landing or takeoff may begin immediately
    }
   return false;                      //Inform the caller that the airplane must wait for its turn to use the landing strip
  }

 /* Notification that an airplane completed its takeoff or landing (called by the Airspace and ParkingArea modules) */
 void ControlTower::completed()
  {
   Enter_Method("completed()");       //Denotes that this member function is callable from other modules (in our case, the Airspace and the ParkingArea)

   //Retrieve the time the oldest airplanes arrived in the holdingQueue (Airspace) and the departQueue(ParkingArea)
   double holdingQueueMaxTime = airspace->getMaxQueueTime();
   double departQueueMaxTime = parkingArea->getMaxQueueTime();

   //Determine the next airplane allowed to use the landing strip for landing or takeoff, if any
   if(holdingQueueMaxTime < 0 && departQueueMaxTime < 0)    //If both queues are empty, set the landing strip as available and wait for the next airplane to arrive from the Airspace
    landingStripOccupied = false;
   else
    if(holdingQueueMaxTime < 0)                             //Otherwise, if just the holdingQueue is empty, inform the ParkingArea that the next plane may takeoff
     parkingArea->go();
    else
     if(departQueueMaxTime < 0)                             //Otherwise, if just the departQueue is empty, inform the Airspace that the next plane may land
      airspace->go();
     else
      if(holdingQueueMaxTime < departQueueMaxTime)          //Otherwise if no queue is empty, assign the landing strip to the airplane with the oldest arrival times in both queues
       airspace->go();                                      //(where in case the oldest planes in the queue share their arrival time, the airplane in the holdingQueue takes the priority)
      else
       parkingArea->go();
  }

}; //namespace
