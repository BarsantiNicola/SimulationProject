#include "Airplane_m.h"
#include "Airspace.h"         //Required to avoid the "Invalid use of incomplete type" error when dereferencing the "airspace" pointer
#include "ParkingArea.h"      //Required to avoid the "Invalid use of incomplete type" error when dereferencing the "parkingarea" pointer
#include "ControlTower.h"

namespace airport
{
 Define_Module(ControlTower);

 /* Module initializing function */
 void ControlTower::initialize()
  {
   landingStripOccupied = false;       //Initialize the landing strip as available

   //this try-catch block is used to initialize the pointers to the Airspace and the ParkingArea addressing the formal possibility of the cast_and_check function raising a "cRunTimeError" exception (which should never happen)
   try
    {
     airspace = check_and_cast<Airspace*>(getModuleByPath("AirTrafficControl.airspace"));
     parkingArea = check_and_cast<ParkingArea*>(getModuleByPath("AirTrafficControl.airport.parkingarea"));
    }
   catch(const cRuntimeError& e)
    {
     getSimulation()->getActiveEnvir()->alert("[FATAL]: \"ControlTower\" module couldn't retrieve the addresses of the \"Airspace\"/\"ParkingArea\" modules");   //Display an error message (halting the simulation in the QTENV interface)
     EV<<e.what()<<endl;                                                                                                                                         //Print the stack trace
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
