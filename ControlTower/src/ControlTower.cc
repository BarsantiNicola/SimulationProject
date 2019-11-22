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

#include "ControlTower.h"
#include "Airplane_m.h"

namespace controltower {

Define_Module(ControlTower);

void ControlTower::initialize()
{
 landingStripOccupied = false;
}

bool ControlTower::notify()
{
 if(!landingStripOccupied)
  {
   landingStripOccupied = true;
   return true;
  }
 return false;
}

void ControlTower::completed()
{
 double airSpaceQueueTime = Airspace.getMaxQueueTime();
 double parkingAreaQueueTime = ParkingArea.getMaxQueueTime();

 if((airSpaceQueueTime!=-1) && (parkingAreaQueueTime!=-1))
  {
   if(airSpaceQueueTime<parkingAreaQueueTime)
    AirSpace.pop();
   else
    ParkingArea.pop();
  }
 else
  if(airSpaceQueueTime == -1)
   ParkingArea.pop();
  else
   if(parkingAreaQueueTime == -1)
    AirSpace.pop();
   else
    landingStripOccupied = false;
}




} //namespace
