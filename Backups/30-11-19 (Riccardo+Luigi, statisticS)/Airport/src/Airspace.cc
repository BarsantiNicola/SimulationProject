#include "Airplane_m.h"
#include "ControlTower.h"

namespace airport {

    Define_Module(Airspace);

    void Airspace::initialize()
    {
        isArrivalRateRandom = par("randomInterArrival").boolValue();
        interArrivalRate = par("interArrivalRate").doubleValue();
        isLandingTimeRandom = par("randomLandingTime").boolValue();
        landingTimeRate = par("landingTimeRate").doubleValue();

        checkHoldingQueue = registerSignal("checkHoldingQueue");
        airplaneLanding = registerSignal("airplaneLanding");

        if(isArrivalRateRandom)
            nextArrival = exponential(interArrivalRate);
        else
            nextArrival = interArrivalRate;
        holdingQueue = new cQueue();
        scheduleAt(nextArrival, new Airplane());
        EV<<"[AIRSPACE] Initialized"<<endl;
    }

    void Airspace::handleMessage(cMessage* airplane){
        controlTower = getModuleByPath("Airport.aerodrome.controltower");
        try{
            if(airplane->isSelfMessage()){ // New airplane arrived

                airplane->setTimestamp(simTime());                       //Set the airplane time arrival in the system
                ((Airplane*)airplane)->setQueueArrival(simTime().dbl()); //Set the airplane time arrival in the Airspace Queue
                holdingQueue->insert(airplane);                          //Insert in the queue
                emit(checkHoldingQueue,holdingQueue->getLength());

                if(check_and_cast<ControlTower*>(controlTower)->notify()){  // If the CT grants, the airplane is free to land
                    // Basically it is the pop() function
                    emit(airplaneLanding,simTime().dbl()-((Airplane*)airplane)->getQueueArrival());
                    if(isLandingTimeRandom)
                        landingTime = exponential(landingTimeRate);
                    else
                        landingTime = landingTimeRate;
                    sendDelayed((Airplane*)holdingQueue->pop(), landingTime, "out");
                }

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
        } catch(const cRuntimeError& e){
            EV<<"check_and_cast() error"<<endl;
            EV<<e.what()<<endl;
        }
    }

    void Airspace::pop()
    {
        Enter_Method("pop()");
        if(isLandingTimeRandom)
            landingTime = exponential(landingTimeRate);
        else
            landingTime = landingTimeRate;

        Airplane* airplane = (Airplane*)holdingQueue->pop();
        emit(airplaneLanding,simTime().dbl()-airplane->getQueueArrival());
        sendDelayed(airplane, landingTime, "out");
    }

    double Airspace::getMaxQueueTime(){
        Enter_Method("getMaxQueueTime()");
        if(holdingQueue->isEmpty())
            return -1;
        return ((Airplane*)(holdingQueue->front()))->getQueueArrival();
    }

    void Airspace::finish(){
        holdingQueue->clear();
        delete(holdingQueue);
    }
}; // namespace
