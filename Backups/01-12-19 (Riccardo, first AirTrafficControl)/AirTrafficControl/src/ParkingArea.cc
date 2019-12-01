#include "Airplane_m.h"
#include "ControlTower.h"

namespace airport
{
 Define_Module(ParkingArea);

 /* Module initializing function */
 void ParkingArea::initialize()
  {
   //Input Parameters Initializations
   isParkingTimeRandom = par("isParkingTimeRandom").boolValue();
   isTakeoffTimeRandom = par("isTakeoffTimeRandom").boolValue();
   parkingTime = par("parkingTime").doubleValue();
   takeoffTime = par("takeoffTime").doubleValue();

   //Statistics-related Signals Initializations
   parkedPlanes = registerSignal("ParkedPlanes");
   departQueueSize = registerSignal("DepartQueueSize");
   departQueueWaitingTime = registerSignal("DepartQueueWaitingTime");

   //Other Members Initializations
   numParked = 0;
   departQueue = new cQueue();
  }


 /* Used to display the number of parked planes and the size of the departing queue in the graphical QTENV interface */
 void ParkingArea::refreshDisplay() const
  {
   char buf[50];
   sprintf(buf,"Parked: %li Queued: %i",numParked, departQueue->getLength());
   getDisplayString().setTagArg("t",0,buf);
  }


 /* Module message handling function */
 void ParkingArea::handleMessage(cMessage* airplane)
  {
   controlTower = getModuleByPath("AirTrafficControl.airport.controltower");    //Retrieve the address of the "controltower" simple module
   try                                                                          //This try block was inserted to catch possible cRuntimeError exceptions raised by the check_and_cast<> function (which should never happen)
    {
     if(airplane->isSelfMessage())                                              //Receiving a self-message means that an airplane has expired its parking time and it's ready for takeoff
      {
       --numParked;                                                             //Decrease the number of parked airplanes
       emit(departQueueSize,(long)departQueue->getLength());                    //Collect a sample of the departing queue length
       if(check_and_cast<ControlTower*>(controlTower)->notify())                //If the Control Tower reports that the plane is available for an immediate takeoff
        {
         EV<<"[ParkingArea]: The airplane N°"<<airplane->getId()+1<<" has finished parking, and the control tower reports that is immediately available for takeoff"<<endl;
         emit(departQueueWaitingTime,0.0);                                      //Collect a sample of the departing queue waiting time (in this particular case, 0)
         if(isTakeoffTimeRandom)                                                //Compute the airplane's takeoff time, depending whether it is constant or random
          nextTakeoffTime = exponential(takeoffTime);
         else
          nextTakeoffTime = takeoffTime;
         sendDelayed((Airplane*)airplane, nextTakeoffTime, "out");              //Start the airplane's takeoff, which will complete in a "nextTakeoffTime" time
        }
       else                                                                     //Otherwise, if the plane is not available for an immediate takeoff
        {
         EV<<"[ParkingArea]: The airplane N°"<<airplane->getId()+1<<" has finished parking, and has been enqueued for takeoff"<<endl;
         ((Airplane*)airplane)->setQueueArrival(simTime().dbl());               //Set the airplane's arrival time in the departing queue
         departQueue->insert(airplane);                                         //Insert the airplane into the departing queue
        }
      }
     else                                                                       //Otherwise an airplane has finished landing from the Airspace
      {
       EV<<"[ParkingArea]: The airplane N°"<<airplane->getId()+1<<" has finished landing, and is entering the parking area"<<endl;
       emit(parkedPlanes,numParked);                                            //Collect a sample of the number of parked planes
       ++numParked;                                                             //Increase the number of parked airplanes
       if(isParkingTimeRandom)                                                  //Compute the airplane's parking time, depending whether it is constant or random
        nextParkingTime = exponential(parkingTime);
       else
        nextParkingTime = parkingTime;
       scheduleAt(simTime() + nextParkingTime, airplane);                       //Schedule when the airplane will expire its parking time
       (check_and_cast<ControlTower*>(controlTower))->completed();              //Inform the Control Tower that the airplane landed successfully
      }
    }
   catch(const cRuntimeError& e)   //Should never happen
    {
     char err[60];

     //Displays an error message (pausing the simulation in the QTENV interface)
     sprintf(err,"WARNING: check_and_cast error, event number: %li", getSimulation()->getEventNumber());
     getSimulation()->getActiveEnvir()->alert(err);

     //Prints the stack trace
     EV<<e.what()<<endl;
    }
  }


 /* Returns the time the oldest plane entered the departing queue, or "-1" if the queue is empty (called by the ControlTower module) */
 double ParkingArea::getMaxQueueTime()
  {
   Enter_Method("getMaxQueueTime()");                                          //Denotes that this member function is callable from other modules (in our case, the Control Tower)
   if(departQueue->isEmpty())
    return -1;
   return ((Airplane*)(departQueue->front()))->getQueueArrival();              //Being a FIFO queue the oldest airplane is the one in front
  }


 /* Starts the takeoff of the oldest plane in the departing queue (called by the ControlTower module) */
 void ParkingArea::go()
  {
   Enter_Method("go()");                                                       //Denotes that this member function is callable from other modules (in our case, the Control Tower)
   Airplane* airplane = (Airplane*)departQueue->pop();                         //Extract the first airplane from the departing queue (which is always the oldest)
   EV<<"[ParkingArea]: The Control Tower notifies that the airplane N°"<<airplane->getId()+1<<" is allowed to takeoff"<<endl;
   emit(departQueueWaitingTime,simTime().dbl()-airplane->getQueueArrival());   //Collect a sample of the departing queue waiting time
   if(isTakeoffTimeRandom)                                                     //Compute the airplane's takeoff time, depending whether it is constant or random
    nextTakeoffTime = exponential(takeoffTime);
   else
    nextTakeoffTime = takeoffTime;
   sendDelayed(airplane, nextTakeoffTime, "out");                              //Start the airplane's takeoff, which will complete in a "nextTakeoffTime" time
  }


 /* Destructor, used to deallocate the module's dynamic resources */
 ParkingArea::~ParkingArea()
  {
   departQueue->clear();
   delete(departQueue);
  }

}; //namespace
