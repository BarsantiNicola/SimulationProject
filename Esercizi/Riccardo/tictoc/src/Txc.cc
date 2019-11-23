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

#include "Txc.h"
#include <string>

namespace tictoc
{

Define_Module(Txc);

void Txc::initialize()
{
 if(par("sendInitialMessage").boolValue())
  {
   cMessage* msg = new cMessage("tictocMsg");
   send(msg, "out");
  }
}

void Txc::handleMessage(cMessage* msg)
{
 // just send back the message we received
 int count;
 double processTime;

 if(msg->isSelfMessage())
  {
     EV<<"["<<getFullName()<<"]: Sending message after processing time"<<endl;
     send(msg,"out");
  }
 else
  {
   //Increase the Counter
   count = par("counter");
   count++;
   par("counter").setIntValue(count);

   if(par("process").boolValue())
    {
     if(!(strcmp(getFullName(),"tic")))  //Tic
      processTime = exponential(2);
     else   //Toc
      processTime = par("processTime").doubleValue();
     EV<<"["<<getFullName()<<"]: Received message from "<<msg->getSenderModule()->getFullName()<<", counter = "<<count<<", scheduling response after "<<processTime<<"ms"<<endl;
     scheduleAt(simTime()+processTime, msg);
    }
   else
    {
     EV<<"["<<getFullName()<<"]: Received message from "<<msg->getSenderModule()->getFullName()<<", counter = "<<count<<", sending back"<<endl;
     send(msg, "out");
    }
  }
}



}; //namespace
