#include "Airplane_m.h"
#include "ControlTower.h"   //Required to avoid the "Invalid use of incomplete type" error when dereferencing the "controlTower" pointer
#include "ParkingArea.h"

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

   //this try-catch block is used to initialize the pointer to the ControlTower addressing the formal possibility of the cast_and_check function raising a "cRunTimeError" exception (which should never happen)
   try
    { controlTower = check_and_cast<ControlTower*>(getModuleByPath("AirTrafficControl.airport.controltower")); }
   catch(const cRuntimeError& e)
    {
     getSimulation()->getActiveEnvir()->alert("[FATAL]: \"ParkingArea\" module couldn't retrieve the address of the \"ControlTower\" module");   //Display an error message (halting the simulation in the QTENV interface)
     EV<<e.what()<<endl;                                                                                                                         //Print the stack trace
    }
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
   if(airplane->isSelfMessage())                                              //Receiving a self-message means that an airplane has expired its parking time and it's ready for takeoff
    {
     emit(parkedPlanes,--numParked);                                            //Collect a sample of the number of parked planes
     if(controlTower->notify())                                               //Notify the Control Tower of the arrival, and if it reports that the plane is available for an immediate takeoff
      {
       EV<<"[ParkingArea]: An airplane has finished parking, and the control tower reports that is immediately available for takeoff"<<endl;
       emit(departQueueSize,0);                                               //Collect a sample of the departing queue length (in this particular case, 0)
       emit(departQueueWaitingTime,0.0);                                      //Collect a sample of the departing queue waiting time (in this particular case, 0)
       if(isTakeoffTimeRandom)                                                //Compute the airplane's takeoff time, depending whether it is constant or random
        nextTakeoffTime = exponential(takeoffTime,1);
       else
        nextTakeoffTime = takeoffTime;
       sendDelayed((Airplane*)airplane, nextTakeoffTime, "out");              //Start the airplane's takeoff, which will complete in a "nextTakeoffTime" time
      }
     else                                                                     //Otherwise, if the plane is not available for an immediate takeoff
      {
       EV<<"[ParkingArea]: An airplane has finished parking, and has been enqueued for takeoff"<<endl;
       ((Airplane*)airplane)->setQueueArrivalTime(simTime().dbl());           //Set the airplane's arrival time into the departing queue
       departQueue->insert(airplane);                                         //Insert the airplane into the departing queue
       emit(departQueueSize,(long)departQueue->getLength());                  //Collect a sample of the departing queue length
      }
    }
   else                                                                       //Otherwise an airplane has finished landing from the Airspace
    {
     EV<<"[ParkingArea]: An airplane has finished landing, and it's parking"<<endl;
     emit(parkedPlanes,++numParked);                                            //Collect a sample of the number of parked planes
     if(isParkingTimeRandom)                                                  //Compute the airplane's parking time, depending whether it is constant or random
      nextParkingTime = exponential(parkingTime,0);
     else
      nextParkingTime = parkingTime;
     scheduleAt(simTime() + nextParkingTime, airplane);                       //Schedule when the airplane will expire its parking time
     controlTower->completed();                                               //Inform the Control Tower that the airplane landed successfully
    }
  }


 /* Returns the time the oldest plane entered the departing queue, or "-1.0" if the queue is empty (called by the ControlTower module) */
 double ParkingArea::getMaxQueueTime()
  {
   Enter_Method("getMaxQueueTime()");                                         //Denotes that this member function is callable from other modules (in our case, the Control Tower)
   if(departQueue->isEmpty())
    return -1.0;
   return ((Airplane*)(departQueue->front()))->getQueueArrivalTime();         //Being a FIFO queue the oldest airplane is the one in front
  }


 /* Starts the takeoff of the oldest plane in the departing queue (called by the ControlTower module) */
 void ParkingArea::go()
  {
   Enter_Method("go()");                                                           //Denotes that this member function is callable from other modules (in our case, the Control Tower)
   Airplane* airplane = (Airplane*)departQueue->pop();                             //Extract the first airplane from the departing queue (which is always the oldest)
   EV<<"[ParkingArea]: The Control Tower notifies that the next airplane is allowed to takeoff"<<endl;
   emit(departQueueWaitingTime,simTime().dbl()-airplane->getQueueArrivalTime());   //Collect a sample of the departing queue waiting time
   emit(departQueueSize,(long)departQueue->getLength());                           //Collect a sample of the departing queue length TODO: Unnecessary?
   if(isTakeoffTimeRandom)                                                         //Compute the airplane's takeoff time, depending whether it is constant or random
    nextTakeoffTime = exponential(takeoffTime,1);
   else
    nextTakeoffTime = takeoffTime;
   sendDelayed(airplane, nextTakeoffTime, "out");                                  //Start the airplane's takeoff, which will complete in a "nextTakeoffTime" time
  }


 /* Destructor, used to deallocate the module's dynamic resources */
 ParkingArea::~ParkingArea()
  {
   departQueue->clear();
   delete(departQueue);
  }

}; //namespace
