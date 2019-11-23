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

#include "Dispatcher.h"

namespace producerconsumer {

Define_Module(Dispatcher);

void Dispatcher::initialize()
{
 routed = 0;
}

void Dispatcher::refreshDisplay() const
{
 char buf[30];
 sprintf(buf,"Routed: %li",routed);     //Updates the displayed "routed" counter
 getDisplayString().setTagArg("t",0,buf);
}

void Dispatcher::handleMessage(cMessage* msg)
{
 EV<<"["<<getFullName()<<"]: Routing response to consumer n°"<<msg->getKind()-1<<endl;
 routed++;                                 //Increases the "routed" counter
 send(msg,"out",(msg->getKind())-2);       //Sends the response message to its target consumer
}

} //namespace
