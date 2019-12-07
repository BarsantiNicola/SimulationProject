
#include "Sky.h"
#include "Airplane_m.h"
Define_Module(Sky);

void Sky::initialize(){

    scheduleArrival();                                          // Schedule first airplane arrival
    numAirplanesStat = registerSignal("numAirplanesStat");      // Statistic for airplanes number
    waitingTimeLanding = registerSignal("waitingTimeLanding");  // Statistic for waiting times

}

void Sky::handleMessage(cMessage *msg){

    if(strcmp(msg->getName(), "arrival") == 0){                 // A new airplane has arrived
        handleArrival();
    }

    if(strcmp(msg->getName(), "landing") == 0){                 // An airplane is ready to land
         handleLanding();
    }

    if(strcmp(msg->getName(), "runwayRequest") == 0){           // Runway request from Takeoff arrived
        handleStopFromTakeoff();
    }

    if(strcmp(msg->getName(), "runwayAvailable") == 0){         // Runway available message from Takeoff arrived
        handleAvailableFromTakeoff();
    }

}

void Sky::scheduleArrival(){

    double time;                                                                    // Time till the next arrival

    if(par("expo").boolValue()){                                                    // Check if times are exponential or constant values
        time = exponential(par("lambda").doubleValue(), 1);                         // Exponential
    }
    else{
        time = par("lambda").doubleValue();                                         // Constant
    }

    scheduleAt(simTime() + time, new cMessage("arrival"));                          // Schedule next arrival after "time"
    EV<<"Sky :: Arrival in "<<time<<endl;

}

void Sky::scheduleLanding(){

    double time;                                                                    // Time the landing is going to take
    usingRunway = true;                                                             // Set runway as used

    if(par("expo").boolValue()){                                                    // Check if times are exponential or constant values
        time = exponential(par("tl").doubleValue(), 2);                             // Exponential time
    }
    else{
        time = par("tl").doubleValue();                                             // Constant time
    }

    cMessage* msg = new cMessage("landing");
    nextL = msg;                                                                    // Keep track of the time to cancel the landing if a simultaneous
    nextLtime = simTime();                                                          // runway request arrives
    scheduleAt(simTime() + time, msg);                                              // Start landing after "time"
    EV<<"Sky :: Landing in "<<time<<endl;

}

void Sky::handleArrival(){

    airplanes.insert(new Airplane());                                               // New airplane queued for landing
    times.push_back(simTime().dbl());                                               // Record arrival time
    if(airplanes.getLength() == 1 && runwayAvailable){                              // If no landing and no takeoff are going on, start landing
        scheduleLanding();
    }

    emit(numAirplanesStat, airplanes.getLength());                                  // Submit new queue length
    EV<<"Sky :: Arrival, Airplanes: "<<airplanes.getLength()<<endl;
    scheduleArrival();                                                              // Schedule next arrival

}

void Sky::handleLanding(){

    usingRunway = false;                                                            // Runway not used anymore
    send((cMessage*)airplanes.pop(), "runway");                                     // Send airplane to runway
    double waiting = simTime().dbl() - times.front();                               // Compute waiting time
    emit(waitingTimeLanding, waiting);                                              // Submit waiting time
    times.erase(times.begin());                                                     // Erase time from vector
    emit(numAirplanesStat, airplanes.getLength());                                  // Submit new queue length
    EV<<"Sky :: Landing, Airplanes: "<<airplanes.getLength()<<" Waiting Time: "<<waiting<<endl;

    if(!runwayAvailable){                                                           // If there's a runway request pending, answer
        send(new cMessage("runwayAvailable"), "outTakeoff");
        EV<<"Sky :: Runway Available Sent"<<endl;
    }

    if(!airplanes.isEmpty() && runwayAvailable){                                    // If there are still airplanes queued, no takeoff or requests going on,
        scheduleLanding();                                                          // start next landing
    }

}

void Sky::handleStopFromTakeoff(){

    runwayAvailable = false;                                                        // Runway request from Takeoff

    if(simTime() - nextLtime == 0){                                                 // Check if there's a landing that has just been scheduled
        usingRunway = false;
        cancelEvent(nextL);                                                         // Cancel it and set the runway free
        EV<<"Sky :: Canceled Landing"<<endl;
    }                                                                               // Runway not available anymore, being used by takeoff

    if(!usingRunway){                                                               // If no landing is going on, answer immediately
        send(new cMessage("runwayAvailable"), "outTakeoff");                        // Send answer
        EV<<"Sky :: Runway Available Sent"<<endl;
    }                                                                               // If a landing is going on, the answer will be sent as soon as it's over

    EV<<"Sky :: Runway Request Received"<<endl;

}

void Sky::handleAvailableFromTakeoff(){                                        // Runway available notification from Takeoff

    runwayAvailable = true;                                                         // (takeoff is done)

    if(!airplanes.isEmpty()){
        scheduleLanding();                                                          // If there are queued airplanes, start a landing
    }

    EV<<"Sky :: Runway Available Received"<<endl;

}



