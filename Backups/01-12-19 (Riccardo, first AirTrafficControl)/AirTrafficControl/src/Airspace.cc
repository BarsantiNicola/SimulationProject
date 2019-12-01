#include "Airplane_m.h"
#include "ControlTower.h"

namespace airport
{
 Define_Module(Airspace);

 /* Module initializing function */
 void Airspace::initialize()
  {
   //Input Parameters Initializations
   isInterArrivalTimeRandom = par("isInterArrivalTimeRandom").boolValue();
   isLandingTimeRandom = par("isLandingTimeRandom").boolValue();
   interArrivalTime = par("interArrivalTime").doubleValue();
   landingTime = par("landingTime").doubleValue();

   //Statistics-related Signals Initializations
   holdingQueueSize = registerSignal("HoldingQueueSize");
   holdingQueueWaitingTime = registerSignal("HoldingQueueWaitingTime");
   airportResponseTime = registerSignal("AirportResponseTime");

   //Other Members Initializations
   holdingQueue = new cQueue();

   //First Airplane arrival
   if(isInterArrivalTimeRandom)                       //Compute the first airplane's arrival time, depending whether it is constant or random
    nextArrival = exponential(interArrivalTime);
   else
    nextArrival = interArrivalTime;
   scheduleAt(nextArrival, new Airplane());           //Schedule the first airplane's arrival at time "nextArrival"
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
    controlTower = getModuleByPath("AirTrafficControl.airport.controltower");      //Retrieve the address of the "controltower" simple module
    try                                                                            //This try block was inserted to catch possible cRuntimeError exceptions raised by the check_and_cast<> function (which should never happen)
     {
      if(airplane->isSelfMessage())                                                //Receiving a self-message means that a new airplane has entered the system
       {
        airplane->setTimestamp(simTime());                                         //Set the airplane arrival time in the system (using the "timestamp" field of the cMessage class, whose semantic is user-defined)
        emit(holdingQueueSize,(long)holdingQueue->getLength());                    //Collect a sample of the holding queue length
        if(check_and_cast<ControlTower*>(controlTower)->notify())                  //If the Control Tower reports that the plane is available for an immediate landing
         {
          EV<<"[Airspace]: The airplane N°"<<airplane->getId()+1<<" has arrived, and the control tower reports that is immediately available for landing"<<endl;
          emit(holdingQueueWaitingTime,0.0);                                       //Collect a sample of the departing queue waiting time (in this particular case, 0)
          if(isLandingTimeRandom)                                                  //Compute the airplane's landing time, depending whether it is constant or random
           nextLandingTime = exponential(landingTime);
          else
           nextLandingTime = landingTime;
          sendDelayed(airplane, nextLandingTime, "out");     //Start the airplane's landing, which will complete in a "nextLandingTime" time
         }
        else                                                                       //Otherwise, if the plane is not available for an immediate landing
         {
          EV<<"[Airspace]: The airplane N°"<<airplane->getId()+1<<" has arrived, and has been enqueued for landing"<<endl;
          ((Airplane*)airplane)->setQueueArrival(simTime().dbl());                 //Set the airplane's arrival time in the holding queue
          holdingQueue->insert(airplane);                                          //Insert the airplane into the holding queue
         }
        if(airplane->getId() < TOTAL_AIRPLANES-1)                                  //If less than TOTAL_AIRPLANES have arrived, schedule the next arrival
         {
          if(isInterArrivalTimeRandom)                                             //Compute the next airplane's arrival time, depending whether it is constant or random
           nextArrival = exponential(interArrivalTime);
          else
           nextArrival = interArrivalTime;
          scheduleAt(simTime() + nextArrival, new Airplane());                     //Schedule the next airplane's arrival
         }
       }
      else                                                                         //Otherwise an airplane has finished its takeoff from the Parking Area
       {
        EV<<"[Airspace]: The airplane N°"<<airplane->getId()+1<<" has taken off and has left the system"<<endl;
        emit(airportResponseTime,simTime().dbl()-airplane->getTimestamp().dbl());  //Collect a sample of the airport system response time
        check_and_cast<ControlTower*>(controlTower)->completed();                  //Inform the Control Tower that the airplane's takeoff is complete
        delete(airplane);                                                          //Remove the airplane from the system
       }
     }
    catch(const cRuntimeError& e)
    {
     char err[60];

     //Displays an error message (pausing the simulation in the QTENV interface)
     sprintf(err,"WARNING: check_and_cast error, event number: %li", getSimulation()->getEventNumber());
     getSimulation()->getActiveEnvir()->alert(err);

     //Prints the stack trace
     EV<<e.what()<<endl;
    }
   }


  /* Returns the time the oldest plane entered the holding queue, or "-1" if the queue is empty (called by the ControlTower module) */
  double Airspace::getMaxQueueTime()
   {
    Enter_Method("getMaxQueueTime()");                                             //Denotes that this member function is callable from other modules (in our case, the Control Tower)
    if(holdingQueue->isEmpty())
     return -1;
    return ((Airplane*)(holdingQueue->front()))->getQueueArrival();                //Being a FIFO queue the oldest airplane is the one in front
   }


  /* Starts the landing of the oldest plane in the holding queue (called by the ControlTower module) */
  void Airspace::go()
   {
    Enter_Method("go()");                                                          //Denotes that this member function is callable from other modules (in our case, the Control Tower)
    Airplane* airplane = (Airplane*)holdingQueue->pop();                           //Extract the first airplane from the holding queue (which is always the oldest)
    EV<<"[Airspace]: The Control Tower notifies that the airplane N°"<<airplane->getId()+1<<" is allowed to land"<<endl;
    emit(holdingQueueWaitingTime,simTime().dbl()-airplane->getQueueArrival());     //Collect a sample of the holding queue waiting time
    if(isLandingTimeRandom)                                                        //Compute the airplane's landing time, depending whether it is constant or random
     nextLandingTime = exponential(landingTime);
    else
     nextLandingTime = landingTime;
    sendDelayed(airplane, nextLandingTime, "out");                                 //Start the airplane's landing, which will complete in a "nextLandingTime" time
   }


  /* Destructor, used to deallocate the module's dynamic resources */
  Airspace::~Airspace()
   {
    holdingQueue->clear();
    delete(holdingQueue);
   }

}; // namespace
