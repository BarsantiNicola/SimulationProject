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

namespace tichubtoc {

Define_Module(Txc);

  void Txc::initialize()
  {
   sent = 0;
   received = 0;
   msgArrived = registerSignal("msgArrived");
   lastSent = 0;
   if(par("sendInitialMessage").boolValue())
    {
     cMessage *msg = new cMessage("tictocMsg");
     send(msg, "out");
    }
  }

  void Txc::refreshDisplay() const
  {
   char buf[40];
   sprintf(buf,"received: %li sent: %li",received,sent);
   getDisplayString().setTagArg("t",0,buf);
  }

  void Txc::handleMessage(cMessage *msg)
  {
   received++;
   emit(msgArrived,simTime()-lastSent);
   lastSent = simTime();
   EV<<"["<<getFullName()<<"]: Received message from "<<msg->getSenderModule()->getFullName()<<", sending back"<<endl;
   send(msg, "out");
   sent++;
  }


}; // namespace
