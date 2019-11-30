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

#include "Chef.h"

namespace barkitchen {

Define_Module(Chef);

void Chef::initialize()
{
 utilizationTime = 0;
 serving = false;
 requestQueue = new cQueue();
 orderArrival = registerSignal("orderArrival");
 orderTime = registerSignal("orderTime");
 remainingTime = registerSignal("remainingTime");
 newProcessing = registerSignal("newProcessing");
}

void Chef::finish()
{
 emit(remainingTime,utilizationTime);
}

void Chef::handleMessage(cMessage* order)
{
 double index;

 if(order->isSelfMessage())
  {
   emit(orderTime,simTime()-order->getCreationTime());
   delete(order);
   if(!requestQueue->isEmpty())
    {
     Order* nextOrd = ((Order*)requestQueue->pop());
     index = nextOrd->getIndex();
     scheduleAt(simTime()+index,nextOrd);
    }
   else
    serving = false;
  }
 else
  {
   index = ((Order*)order)->getIndex();
   emit(orderArrival,simTime());
   utilizationTime = utilizationTime + index;

   if(!serving)
    {
     scheduleAt(simTime()+index,order);
     serving = true;
    }
   else
    requestQueue->insert(order);

   emit(newProcessing,requestQueue->getLength());
  }
}

} //namespace
