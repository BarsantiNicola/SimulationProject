#include "Txc.h"

namespace mytictoc {

    Define_Module(Txc);

    void Txc::initialize(){
        countSignal = registerSignal("Msg");
        interProcTimeSignal = registerSignal("_interProcTime");
        count = 0;
        interProcTime = 0;

        if (par("sendInitialMessage").boolValue()){
            cMessage *msg = new cMessage("tictocMsg");
            send(msg, "out");
        }
    }

    void Txc::handleMessage(cMessage *msg){
        // just send back the message we received
        if (msg->isSelfMessage() || par("enable").boolValue()){
            ++count;
            if(par("enable").boolValue()){
                EV<<getName()<<": msg #"<<count;
                emit(countSignal, count);
            }
            send(msg, "out");
        }else{
            double procTime = 0;
            if (strcmp(getName(), "tic")){
                procTime = uniform(0,5);
            } else{
                procTime = par("procTime").doubleValue();
            }
            EV<<getName()<<": procTime "<<procTime;
            scheduleAt( simTime() + procTime , msg );
            ++count;
            EV<<getName()<<": msg #"<<count;
            emit(countSignal, count);
            emit(interProcTimeSignal, simTime()-interProcTime);
            interProcTime = simTime().dbl();
        }
    }
}; // namespace
