#include "Airplane_m.h"
#include "ControlTower.h"   //Required to avoid "Invalid use of incomplete type" when dereferencing the "controlTower" pointer
#include "Airspace.h"

namespace airport
{
 Define_Module(Airspace);

 /* Module initializing function */
 void Airspace::initialize()
  {

   // Warmup time
   double warmup = par("warmup");
   double scaling = par("scaling");
   getSimulation()->setWarmupPeriod(scaling * warmup);

   //Input Parameters Initializations
   isInterArrivalTimeRandom = par("isInterArrivalTimeRandom").boolValue();
   isLandingTimeRandom = par("isLandingTimeRandom").boolValue();
   interArrivalTime = par("interArrivalTime").doubleValue();
   landingTime = par("landingTime").doubleValue();
   totalSamples = par("totalSamples").intValue();

   //Statistics-related Signals Initializations
   holdingQueueSize = registerSignal("HoldingQueueSize");
   holdingQueueWaitingTime = registerSignal("HoldingQueueWaitingTime");
   airportResponseTime = registerSignal("AirportResponseTime");

   //Other Members Initializations
   departedPlanes = 0;
   holdingQueue = new cQueue();

   //this try-catch block is used to initialize the pointer to the ControlTower addressing the formal possibility of the cast_and_check function raising a "cRunTimeError" exception (which should never happen)
   try
    { controlTower = check_and_cast<ControlTower*>(getModuleByPath("AirTrafficControl.airport.controltower")); }
   catch(const cRuntimeError& e)
    {
     getSimulation()->getActiveEnvir()->alert("[FATAL]: \"Airspace\" module couldn't retrieve the address of the \"ControlTower\" module");   //Display an error message (halting the simulation in the QTENV interface)
     EV<<e.what()<<endl;                                                                                                                      //Print the stack trace
     return;                                                                                                                                  //Halts the simulation by preventing the creation of the first event
    }

   //Schedule the first event. i.e. the first airplane's arrival
   Airplane* firstPlane = new Airplane();
   if(isInterArrivalTimeRandom)                                                                                                               //Compute the first airplane's arrival time, depending whether it is constant or random
    nextArrival = exponential(interArrivalTime,0);
   else
    nextArrival = interArrivalTime;
   scheduleAt(nextArrival, firstPlane);                                                                                                       //Schedule the first airplane's arrival at time "nextArrival"
  }


  /* Used to display the size of the departing queue in the graphical QTENV interface */
  void Airspace::refreshDisplay() const
   {
    char buf[25];
    sprintf(buf,"Queued: %u",holdingQueue->getLength());
    getDisplayString().setTagArg("t",0,buf);
   }


  /* Module message handling function */
  void Airspace::handleMessage(cMessage* airplane)
   {
    if(airplane->isSelfMessage())                                                //Receiving a self-message means that a new airplane has entered the system
     {
      airplane->setTimestamp(simTime());                                         //Set the airplane arrival time in the system (using the "timestamp" field of the cMessage class, whose semantic is user-defined)
      emit(holdingQueueSize,(long)holdingQueue->getLength());                    //Collect a sample of the holding queue length
      if(controlTower->notify())                                                 //Notify the Control Tower of the arrival, and if it reports that the plane is available for an immediate landing
       {
        EV<<"[Airspace]: A new airplane has arrived, and the control tower reports that is immediately available for landing"<<endl;
        emit(holdingQueueWaitingTime,0.0);                                       //Collect a sample of the departing queue waiting time (in this particular case, 0)
        if(isLandingTimeRandom)                                                  //Compute the airplane's landing time, depending whether it is constant or random
         nextLandingTime = exponential(landingTime,1);
        else
         nextLandingTime = landingTime;
        sendDelayed(airplane, nextLandingTime, "out");                           //Start the airplane's landing, which will complete in a "nextLandingTime" time
       }
      else                                                                       //Otherwise, if the plane is not available for an immediate landing
       {
        EV<<"[Airspace]: A new airplane has arrived, and has been enqueued for landing"<<endl;
        ((Airplane*)airplane)->setQueueArrivalTime(simTime().dbl());             //Set the airplane's arrival time into the holding queue
        holdingQueue->insert(airplane);                                          //Insert the airplane into the holding queue
       }
      Airplane* nextPlane = new Airplane();                                      //Create the next airplane
      if(isInterArrivalTimeRandom)                                               //Compute the next airplane's arrival time, depending whether it is constant or random
       nextArrival = exponential(interArrivalTime,0);
      else
       nextArrival = interArrivalTime;
      scheduleAt(simTime() + nextArrival, nextPlane);                     //Schedule the next airplane's arrival
     }
    else                                                                         //Otherwise an airplane has finished its takeoff from the Parking Area
     {
      EV<<"[Airspace]: An airplane has taken off and has left the system"<<endl;
      emit(airportResponseTime,simTime().dbl()-airplane->getTimestamp().dbl());  //Collect a sample of the airport system response time
      delete(airplane);                                                          //Remove the airplane from the system
      if(++departedPlanes < totalSamples)                                        //If less than "totalSamples" airplanes have departed the system, i.e. less that "totalSamples" samples have been collected
       controlTower->completed();                                                //Inform the Control Tower that the airplane's takeoff is complete
      else                                                                       //Otherwise, end the simulation
       endSimulation();
     }
   }


  /* Returns the time the oldest plane entered the holding queue, or "-1.0" if the queue is empty (called by the ControlTower module) */
  double Airspace::getMaxQueueTime()
   {
    Enter_Method("getMaxQueueTime()");                                             //Denotes that this member function is callable from other modules (in our case, the Control Tower)
    if(holdingQueue->isEmpty())
     return -1.0;
    return ((Airplane*)(holdingQueue->front()))->getQueueArrivalTime();            //Being a FIFO queue the oldest airplane is the one in front
   }


  /* Starts the landing of the oldest plane in the holding queue (called by the ControlTower module) */
  void Airspace::go()
   {
    Enter_Method("go()");                                                           //Denotes that this member function is callable from other modules (in our case, the Control Tower)
    Airplane* airplane = (Airplane*)holdingQueue->pop();                            //Extract the first airplane from the holding queue (which is always the oldest)
    EV<<"[Airspace]: The Control Tower notifies that the next airplane is allowed to land"<<endl;
    emit(holdingQueueWaitingTime,simTime().dbl()-airplane->getQueueArrivalTime());  //Collect a sample of the holding queue waiting time
    emit(holdingQueueSize,(long)holdingQueue->getLength());                         //Collect a sample of the holding queue length TODO: Unnecessary?
    if(isLandingTimeRandom)                                                         //Compute the airplane's landing time, depending whether it is constant or random
     nextLandingTime = exponential(landingTime,1);
    else
     nextLandingTime = landingTime;
    sendDelayed(airplane, nextLandingTime, "out");                                  //Start the airplane's landing, which will complete in a "nextLandingTime" time
   }


  /* Destructor, used to deallocate the module's dynamic resources */
  Airspace::~Airspace()
   {
    holdingQueue->clear();
    delete(holdingQueue);
   }

}; // namespace
