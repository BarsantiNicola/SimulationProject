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

#include "Producer.h"

namespace producerconsumer {

Define_Module(Producer);

void Producer::initialize()
{
 queued = 0;
 produced = 0;
 newRequest = registerSignal("newRequest");
 productionRate = par("productionRate").doubleValue();
}

 void Producer::refreshDisplay() const
{
 char buf[50];
 sprintf(buf,"Produced: %li Queued: %li",produced,queued);     //Updates the displayed "Produced" and "Queued" counters
 getDisplayString().setTagArg("t",0,buf);
}

void Producer::handleMessage(cMessage *msg)
{
 if(msg->isSelfMessage())           //A request message has been produce, send to the dispatcher node
  {
   EV<<"["<<getFullName()<<"]: Sending response to consumer n°"<<msg->getKind()-1<<endl;
   produced++;                      //Updates the "produced" counter
   queued--;                        //Updates the "queued" counter
   send(msg,"out");                 //Send the message to the dispatcher node
  }
 else
  {
   EV<<"["<<getFullName()<<"]: Received request from consumer n°"<<msg->getKind()-1<<endl;
   cMessage* respMsg = new cMessage("Response Message",msg->getKind());  //Create the response message
   respMsg->setTimestamp(msg->getTimestamp());                           //Set the message's timestamp to the original timestamp
   queued++;
   emit(newRequest,queued);
   scheduleAt(simTime()+exponential(productionRate),respMsg);
   delete(msg);                                                          //Delete the request message
  }
}

} //namespace
