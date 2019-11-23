/*
 * Hub.h
 *
 *  Created on: Nov 18, 2019
 *      Author: rickydenton
 */

#ifndef HUB_H_
#define HUB_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace tichubtoc {

/**
 * Implements the Hub simple module. See the NED file for more information.
 */
class Hub : public cSimpleModule
{
 private:
  long int sent;
  long int received;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual void refreshDisplay() const;
};


}; // namespace
#endif /* HUB_H_ */
