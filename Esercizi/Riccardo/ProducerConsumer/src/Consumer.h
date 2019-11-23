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

#ifndef __PRODUCERCONSUMER_TCX_H
#define __PRODUCERCONSUMER_TCX_H

#include <omnetpp.h>
#define MAX_REQUESTS 1000000

using namespace omnetpp;

namespace producerconsumer {

class Consumer : public cSimpleModule
{
 private:
  //Used to collect the total number and the average response time of the requests
  simsignal_t responseArrived;

  //Total number of requests consumed
  long int requested;
  long int consumed;

  //Time of last request
  double reqTime;

  //Normal request rate parameters
  double reqMean;
  double reqVariance;

 protected:
  virtual void initialize();
  virtual void refreshDisplay() const;
  virtual void handleMessage(cMessage *msg);
};

}; // namespace

#endif
