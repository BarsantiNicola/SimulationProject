
#include "Parking.h"
#include "Airplane_m.h"
Define_Module(Parking);

void Parking::initialize(){

    numAirplanesStat = registerSignal("numAirplanesStat");              // Statistic for airplanes number

}

void Parking::handleMessage(cMessage *msg){

    if(msg->isSelfMessage()){                                           // An airplane must leave the parking for takeoff
        numAirplanes--;                                                 // Decrement airplanes in parking counter
        emit(numAirplanesStat, numAirplanes);                           // Submit new number of airplanes in parking
        EV<<"Parking :: Departure, Airplanes "<<numAirplanes<<endl;
        send((cMessage*)new Airplane(), "takeOff");                     // Send airplane to takeoff
    }
    else{                                                               // An airplane arrives from runway
        numAirplanes++;                                                 // Increment airplanes counter
        emit(numAirplanesStat, numAirplanes);                           // Submit new number of airplanes in parking
        double time;                                                    // Time the new airplane has to wait

        if(par("expo").boolValue()){                                    // Check if waiting time is constant or exponential
            time = exponential(par("tp").doubleValue());                // Exponential time
        }
        else{
            time = par("tp").doubleValue();                                                 // Constant time
        }
        scheduleAt(simTime() + time, new cMessage(""));                     // Schedule airplane departure from parking
        EV<<"Parking :: Arrival, Airplanes: "<<numAirplanes<<endl;
    }

}



