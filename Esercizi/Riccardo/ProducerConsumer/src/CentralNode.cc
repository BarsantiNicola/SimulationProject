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

#include "CentralNode.h"

namespace producerconsumer
{
 Define_Module(CentralNode);

 void CentralNode::initialize()
 {
  nextProducer=0;
  routed = 0;
 }

 void CentralNode::refreshDisplay() const
 {
  char buf[30];
  sprintf(buf,"Routed: %li",routed);        //Updates the displayed "routed" counter
  getDisplayString().setTagArg("t",0,buf);
 }

 void CentralNode::handleMessage(cMessage* msg)
 {
  EV<<"["<<getFullName()<<"]: Routing request to producer n°"<<nextProducer<<endl;
  send(msg, "out", nextProducer);                  //Send to the producer
  nextProducer = (nextProducer+1)%2;               //Set the next producer
  routed++;                                        //Increments the "routed" counter
 }

} /* namespace producerconsumer */
