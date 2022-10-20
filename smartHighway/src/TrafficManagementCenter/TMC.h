#ifndef TMC_H_
#define TMC_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "Messaging/RSU_Data_m.h"

// Expected message types
enum {
    TMC_DATA_MSG = 0,
    TMC_TIMER_MSG = 1,
};
#define RL_INTERVAL 1  // Time between receiving RSU data and performing a computation
#define TMC_VERBOSE 1

class parkingLotData : public cObject {
public:
    parkingLotData(std::string nodeId, int cap, int occ) {
        lotID = nodeId;
        capacity = cap;
        occupancy = occ;
    }
    parkingLotData(void) {
        lotID = "genericParkingArea";
        capacity = 0;
        occupancy = 0;
    }
    std::string lotID;
    int capacity;
    int occupancy;
};

namespace veins {


/*
 * TMC will receive information from RSUs periodically. Every set amount of time,
 * the TMC will consult an RL agent to update signal timings and send broadcasts.
 */
class TMC : public cSimpleModule {
public:
    TMC(void);
    ~TMC(void);
protected:
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override;
    void finish(void) override;
    void stringListFromParam(std::vector<std::string> &list, const char *parName);

    // Determine the control output for the system (signal timings, broadcast to reroute)
    void computeAction(void);
    // Control: update signal timing for a particular RSU
    void updateSignalTiming(void);
    // Control: tell RSU to broadcast vehicles to reroute
    void broadcastReroute(int targetRSU);
    // Data: sample the availability of registered park n rides
    void parkingLotStatus(void);

private:
    // need some sort of queue to hold RSU data
    cQueue *rsuData;
    // need some sort of queue to hold parking data
    cQueue *parkingData;
    std::vector<std::string> parkingLotList;
    cMessage *control_timer;  // Recurring self message for control updates
};

}

#endif
