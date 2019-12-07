
#include "TakeOff.h"

Define_Module(TakeOff);

void TakeOff::initialize(){

    numAirplanesStat = registerSignal("numAirplanesStat");                          // Statistic for airplanes number
    waitingTimeTakeOff = registerSignal("waitingTimeTakeOff");                      // Statistic for airplanes' waiting time
}

void TakeOff::handleMessage(cMessage *msg){

    if(msg->isSelfMessage()){                                                       // A takeoff is over
        handleTakeOff();
    }
    else{

        if(msg->arrivedOn("park")){                                                 // New airplane arrived from parking
            handleAirplaneFromParking(msg);
        }

        if(msg->arrivedOn("inSky")){                                            // Runway available from Sky
            handleOkFromSky();
        }

    }

}


void TakeOff::handleAirplaneFromParking(cMessage* msg){

    airplanes.insert(msg);                                                          // Queue new airplane
    EV<<"TakeOff :: Arrival, Airplanes "<<airplanes.getLength()<<endl;
    times.push_back(simTime().dbl());                                               // Record arrival time of airplane
    if(!runwayAvailable){                                                           // If runway is not available,
                                                                                    // and this is the first airplane
        if(airplanes.getLength() == 1){                                             // send runway request to Sky
            send(new cMessage("runwayRequest"), "outSky");
            EV << "TakeOff :: Runway Request Sent"<<endl;
        }

    }
    else{

        if(airplanes.getLength() == 1){                                             // If runway is available and this is the
            scheduleTakeOff();                                                      // first airplane, start takeoff
        }

    }

    emit(numAirplanesStat, airplanes.getLength());                                  // Submit new queue length

}

void TakeOff::scheduleTakeOff(){

    double time;                                                                    // Time takeoff is going to take

    if(par("expo").boolValue()){                                                    // Check if time is exponential or constant
        time = exponential(par("to").doubleValue());       // Exponential
    }
    else{
        time = par("to").doubleValue();                                             // Constant
    }

    scheduleAt(simTime() + time, new cMessage("takeOff"));                          // Start takeoff
    EV<<"TakeOff :: Takeoff in "<<time<<endl;

}

void TakeOff::handleOkFromSky(){

    runwayAvailable = true;                                                         // Runway available arrived from Sky
    EV<<"TakeOff :: Runway Available Received"<<endl;
    scheduleTakeOff();                                                              // Start takeoff (surely there are
                                                                                    // airplanes queued if takeoff requested
}                                                                                   // the runway)

void TakeOff::handleTakeOff(){

    send((cMessage*)airplanes.pop(),"takeOff");                                     // Send airplane to runway
    emit(numAirplanesStat, airplanes.getLength());                                  // Submit new queue length
    double waiting = simTime().dbl() - times.front();                               // Compute waiting time for departing airplane
    EV<<"TakeOff :: TakeOff, Airplanes: "<<airplanes.getLength()<<" Waiting Time: "<<waiting<<endl;
    emit(waitingTimeTakeOff, waiting);                                              // Submit waiting time
    times.erase(times.begin());                                                     // Delete time from vector
    if(!airplanes.isEmpty()){                                                       // If airplanes queue is not empty, start
        scheduleTakeOff();                                                          // next takeoff
    }
    else{
        runwayAvailable = false;                                                    // If queue is empty, notify Sky
        send(new cMessage("runwayAvailable"), "outSky");                           // that runway is available
        EV<<"TakeOff :: Runway Available Sent"<<endl;
    }

}


