#ifndef TMC_H_
#define TMC_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace veins {

class TMC : public cSimpleModule {
protected:
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override;
    void finish(void) override;

    // Determine the control output for the system (signal timings, broadcast to reroute)
    void computeAction(void);
    // Control: update signal timing for a particular RSU
    void updateSignalTiming(void);
    // Control: tell RSU to broadcast vehicles to reroute
    void broadcastReroute(void);
    // Data: sample the availability of registered park n rides
    void parkingLotStatus(void);
    // Data: sample the position and velocities of cars near RSUs
    void collectTrafficInfo(void);
};

}

#endif
