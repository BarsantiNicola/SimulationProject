#include "Airspace.h"
#include "Airplane_m.h"
#include "ControlTower.h"

namespace airport {

    Define_Module(Airspace);

    void Airspace::initialize(){
        isArrivalRateRandom = par("randomInterArrival").boolValue();
        interArrivalRate = par("interArrivalRate").doubleValue();
        isLandingTimeRandom = par("randomLandingTime").boolValue();
        landingTimeRate = par("landingTimeRate").doubleValue();

        if(isArrivalRateRandom)
            nextArrival = exponential(interArrivalRate);
        else
            nextArrival = interArrivalRate;
        holdingQueue = new cQueue();
        scheduleAt(nextArrival, new Airplane());
        EV<<"[AIRSPACE] Initialized"<<endl;
    }

    void Airspace::finish(){
        holdingQueue->clear();
        delete(holdingQueue);
    }

    void Airspace::handleMessage(cMessage* airplane){
        controlTower = getModuleByPath("Airport.aerodrome.controltower");
        if(airplane->isSelfMessage()){   //New airplane arrived

            ((Airplane*)airplane)->setQueueArrival(simTime().dbl());
            holdingQueue->insert(airplane);

            if(check_and_cast<ControlTower*>(controlTower)->notify())
                pop();

            if(airplane->getId() < 1000000000){  //Stop auto-generation at the 1G plane
                if(isArrivalRateRandom)
                    nextArrival = exponential(interArrivalRate);
                else
                    nextArrival = interArrivalRate;
                scheduleAt(simTime() + nextArrival, new Airplane());
            }
        }else{
            check_and_cast<ControlTower*>(controlTower)->completed();
            delete(airplane);
        }
    }

    void Airspace::pop(){
        Enter_Method("pop()");
        Airplane* airplane = (Airplane*)holdingQueue->pop();

        if(isLandingTimeRandom)
            landingTime = exponential(landingTimeRate);
        else
            landingTime = landingTimeRate;
        sendDelayed(airplane, landingTime, "out");
    }

    double Airspace::getMaxQueueTime(){
        Enter_Method("getMaxQueueTime()");
        if(holdingQueue->isEmpty())
            return -1;
        else
            return ((Airplane*)(holdingQueue->front()))->getQueueArrival();
    }
}; // namespace
