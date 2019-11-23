//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Consumer.h"

namespace producerconsumer {

Define_Module(Consumer);

void Consumer::initialize()
{
 //Initializations
 responseArrived = registerSignal("responseArrived");
 reqTime = 0;
 requested = 0;
 consumed = 0;
 reqMean = par("reqMean").doubleValue();
 reqVariance = par("reqVariance").doubleValue();

 cMessage* msg = new cMessage("Request Message",this->getId());    //Create a new request message
 msg->setTimestamp();                                              //Set the request message creation time to the current simTime
 scheduleAt(std::max((double)0,normal(reqMean,reqVariance)), msg); //Schedule the new request after a normal time
}

void Consumer::refreshDisplay() const
{
 char buf[50];
 sprintf(buf,"Requested: %li Consumed: %li",requested, consumed);    //Updates the displayed "Requested" and "Consumed" counters
 getDisplayString().setTagArg("t",0,buf);
}

void Consumer::handleMessage(cMessage* msg)
{
 if(msg->isSelfMessage())       //Ready to send a new request
  {
   requested++;                 //Increases the requested counter
   EV<<"["<<getFullName()<<"]: Sending request n°"<<requested+1<<endl;
   send(msg,"out");             //Send the request to the central node C
   if(requested<MAX_REQUESTS)
    {
     cMessage* newMsg = new cMessage("Request Message",this->getId());                //Create a new request message
     newMsg->setTimestamp();
     double next = std::max((double)0,normal(reqMean,reqVariance));
     EV<<"Next: "<<next<<endl;
     scheduleAt(simTime() + next, newMsg); //Schedule the new request after a normal time
    }
  }
 else                        //We have received a message from the dispatcher node D
 {
  EV<<"["<<getFullName()<<"]: Received response from dispatcher"<<endl;
  emit(responseArrived,simTime()-msg->getTimestamp());   //Emit the responseArrived signal for statistics
  consumed++;                                            //Increase the "consumed" counter
  delete(msg);                                           //Consume the message
 }
}

}; // namespace
