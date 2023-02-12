/*
 * BusApp.h
 *
 *  Created on: Feb 9, 2023
 *      Author: tjzhang
 */

#ifndef CARAPPLICATION_BASICCARAPP_H_
#define CARAPPLICATION_BASICCARAPP_H_

#include "veins/base/modules/BaseModule.h"
#include "veins/base/connectionManager/ChannelAccess.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"

namespace veins {

/* Lightweight class that provides a TraCI interface for a SUMO vehicle that doesn't need networking capability */

class BasicCarApp : public BaseModule {
public:
    BasicCarApp();
    virtual ~BasicCarApp();
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override {
        delete(msg);
    };
    void finish(void) override;

    void receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details) override;

protected:
    /* pointers will be set when used with TraCIMobility */
    TraCIMobility* mobility;
    TraCICommandInterface* traci;
    TraCICommandInterface::Vehicle* traciVehicle;

    /** @brief this function is called every time the vehicle receives a position update signal */
    virtual void handlePositionUpdate(cObject* obj);
};
} /* namespace veins */

#endif /* CARAPPLICATION_BASICCARAPP_H_ */
