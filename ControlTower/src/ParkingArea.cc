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

#include "ParkingArea.h"
#include "Airplane_m.h"

namespace controltower {

Define_Module(ParkingArea);

void ParkingArea::initialize()
{
 isParkingTimeRandom = par("isParkingTimeRandom").boolValue();
 parkingTimeRate = par("parkingTimeRate").doubleValue();
 isTakeoffTimeRandom = par("isTakeoffTimeRandom").boolValue();
 takeoffTimeRate = par("takeoffTimeRate").doubleValue();
 departQueue = new cQueue();
 numParked = 0;
}

void ParkingArea::finish()
{
 departQueue->clear();
 delete(departQueue);
}

void ParkingArea::handleMessage(Airplane* airplane)
{
 if(airplane->isSelfMessage())   //New airplane arrived
  {
   airplane->setQueueArrival(simTime().dbl());
   departQueue->insert(airplane);
   numParked--;
   if(controlTower.notify())
    pop();
  }
 else
  {
   if(isParkingTimeRandom)
    parkingTime = exponential(parkingTimeRate);
   else
    parkingTime = parkingTimeRate;
   scheduleAt(simTime().dbl()+parkingTime, airplane);
   numParked++;
   ControlTower.completed();
  }
}


void ParkingArea::pop()
{
 Airplane* airplane = (Airplane*)departQueue->pop();

 if(isTakeoffTimeRandom)
  takeoffTime = exponential(takeoffTimeRate);
 else
  takeoffTime = takeoffTimeRate;
 send(airplane,"out",takeoffTime);
}

double ParkingArea::getMaxQueueTime()
{
 if(departQueue->isEmpty())
  return -1;
 else
  return ((Airplane*)(departQueue->front()))->getQueueArrival();
}

} //namespace
