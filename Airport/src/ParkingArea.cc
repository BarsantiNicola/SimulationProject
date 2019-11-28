#include "ParkingArea.h"
#include "Airplane_m.h"
#include "ControlTower.h"

namespace airport {

    Define_Module(ParkingArea);

    void ParkingArea::initialize(){
        isParkingTimeRandom = par("isParkingTimeRandom").boolValue();
        parkingTimeRate = par("parkingTimeRate").doubleValue();
        isTakeoffTimeRandom = par("isTakeoffTimeRandom").boolValue();
        takeoffTimeRate = par("takeoffTimeRate").doubleValue();
        departQueue = new cQueue();
        numParked = 0;
        EV<<"[PARKING AREA] Initialized"<<endl;
    }

    void ParkingArea::finish(){
        departQueue->clear();
        delete(departQueue);
    }

    void ParkingArea::handleMessage(cMessage* airplane){
        controlTower = getModuleByPath("Airport.aerodrome.controltower");
        if(airplane->isSelfMessage()){   //New airplane arrived
            ((Airplane*)airplane)->setQueueArrival(simTime().dbl());
            departQueue->insert(airplane);
            numParked--;
            if(check_and_cast<ControlTower*>(controlTower)->notify())
                pop();
        }
        else{
            if(isParkingTimeRandom)
                parkingTime = exponential(parkingTimeRate);
            else
                parkingTime = parkingTimeRate;
            scheduleAt(simTime() + parkingTime, airplane);
            numParked++;
            (check_and_cast<ControlTower*>(controlTower))->completed();
        }
    }

    void ParkingArea::pop(){
        Enter_Method("pop()");
        Airplane* airplane = (Airplane*)departQueue->pop();
        if(isTakeoffTimeRandom)
            takeoffTime = exponential(takeoffTimeRate);
        else
            takeoffTime = takeoffTimeRate;
        sendDelayed(airplane, takeoffTime, "out");
    }

    double ParkingArea::getMaxQueueTime(){
        Enter_Method("getMaxQueueTime()");
        if(departQueue->isEmpty())
            return -1;
        else
            return ((Airplane*)(departQueue->front()))->getQueueArrival();
    }
}; //namespace
