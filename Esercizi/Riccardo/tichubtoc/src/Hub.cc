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

#include "Hub.h"

namespace tichubtoc {

Define_Module(Hub);

void Hub::initialize()
{
 sent = 0;
 received = 0;
 return;
}

void Hub::refreshDisplay() const
{
 char buf[40];
 sprintf(buf,"rec: %li sent: %li",received,sent);
 getDisplayString().setTagArg("t",0,buf);
}


void Hub::handleMessage(cMessage* msg)
{
 int arrived = msg->getArrivalGate()->getIndex();  //index of the gate in the gate array the message arrived from
 int toSend;                                       //index of the gate in the gate array where the message must be sent from

 received++;

 if(par("randHub").boolValue())
  {
   toSend = intuniform(0,1);
  }
 else
  toSend = (arrived+1)%2;

 EV<<"["<<getFullName()<<"]: message arrived on gate "<<arrived<<", forwarding on gate "<<toSend<<endl;
 send(msg, "out", toSend);
 sent++;
}

} /* namespace tichubtoc */
