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

#include "Airspace.h"
#include "Airplane_m.h"
#include "ControlTower.h"


namespace controltower {

Define_Module(Airspace);

void Airspace::initialize()
{
 isArrivalRateRandom = par("randomInterarrival").boolValue();
 interArrivalRate = par("interArrivalRate").doubleValue();
 isLandingTimeRandom = par("randomLandingTime").boolValue();
 landingTimeRate = par("landingTimeRate").doubleValue();


 if(isArrivalRateRandom)
  nextArrival = exp(interArrivalRate);
 else
  nextArrival = interArrivalRate;
 holdingQueue = new cQueue();
 scheduleAt(nextArrival,new Airplane());
}

void Airspace::finish()
{
 holdingQueue->clear();
 delete(holdingQueue);
}

void Airspace::handleMessage(Airplane* airplane)
{
 if(airplane->isSelfMessage())   //New airplane arrived
  {
   airplane->setQueueArrival(simTime().dbl());
   holdingQueue->insert(airplane);

   if(controlTower.notify())
    pop();
   if(airplane->getId() < 1000000000)  //Stop auto-generation at the 1G plane
    {
     if(isArrivalRateRandom)
      nextArrival = exp(interArrivalRate);
     else
      nextArrival = interArrivalRate;
     scheduleAt(simTime().dbl()+nextArrival,new Airplane());
    }
  }
 else
  {
   //controlTower.completed();
   delete(airplane);
  }
}

void Airspace::pop()
{
 Airplane* airplane = (Airplane*)holdingQueue->pop();

 if(isLandingTimeRandom)
  landingTime = exp(landingTimeRate);
 else
  landingTime = landingTimeRate;
 send(airplane,"out",landingTime);
}

double Airspace::getMaxQueueTime()
{
 if(holdingQueue->isEmpty())
  return -1;
 else
  return ((Airplane*)(holdingQueue->front()))->getQueueArrival();
}



}; // namespace
