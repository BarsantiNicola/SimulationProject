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

#include "Waitress.h"

namespace barkitchen {

Define_Module(Waitress);

void Waitress::initialize()
{
 Order* ord = new Order();
 ord->setIndex(uniform(0,3));
 ord->setCreationTime(0.0);
 scheduleAt(uniform(0,3),ord);
}

void Waitress::handleMessage(cMessage* ord)
{
 EV<<"[Waitress]: Sending new order"<<endl;
 send(ord, "out");
 Order* newOrd = new Order();
 newOrd->setIndex(uniform(0,3));
 newOrd->setCreationTime(simTime().dbl());
 scheduleAt(simTime()+uniform(0,3),newOrd);
}


}; // namespace
