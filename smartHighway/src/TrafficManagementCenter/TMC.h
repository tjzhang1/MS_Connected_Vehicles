#ifndef TMC_H_
#define TMC_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "protobuf/veinsgym.pb.h"
#include "VeinsGym/GymConnection.h"
#include "Messaging/RSU_Data_m.h"
#include "Messaging/ParkingReroute_m.h"
#include "Messaging/UpdateMeteringRate_m.h"
#include "Messaging/PayloadReward_m.h"
#include "Messaging/ProbeVehicleData_m.h"

// Expected message types
enum {
    TMC_DATA_MSG = 0,
    TMC_TIMER_MSG = 1,
    TMC_BUFFERED_RWD_MSG = 2,
};
#define RL_INTERVAL 1  // Time between receiving RSU data and performing a computation
#define TMC_VERBOSE 0
#define DEBUG_REWARD 1
#define THROUGHPUT_WEIGHT 1.0/60.0  // 1 car per second for 60s = 1/60
#define TARGET_TIME 560.0   // in seconds
#define DELAY_WEIGHT -1.0/TARGET_TIME
#define CO2_WEIGHT -0.0000001

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

typedef struct {
    int hwyThroughput;
    simtime_t accumTravelTime;
    double accumCO2Emissions;
} rewards_t;

namespace veins {


/*
 * TMC will receive information from RSUs periodically. Every set amount of time,
 * the TMC will consult an RL agent to update signal timings and send broadcasts.
 */
class TMC : public cSimpleModule {
public:
    TMC(void);
    ~TMC(void);

    rewards_t globalReward = {0,SimTime::ZERO,0};
    rewards_t bufferedHOVReward = {0,SimTime::ZERO,0};
    rewards_t bufferedVehReward = {0,SimTime::ZERO,0};
    int parkingSpaces = 400;
    const int actionRSU[5] = {
        0,  // I605_EXIT
        3,  // I605_SB_ENTER
        4,  // LAKEWOOD_EXIT
        6,  // LAKEWOOD_SB_ENTER
        7,  // PARAMOUNT_EXIT
    };

protected:
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override;
    void finish(void) override;
    void stringListFromParam(std::vector<std::string> &list, const char *parName);

    // Determine the control output for the system (signal timings, broadcast to reroute)
    void computeAction(void);
    // Control: update signal timing for a particular RSU
    void updateSignalTiming(int targetRM, double meterRate);
    // Control: tell RSU to broadcast vehicles to reroute
    void broadcastReroute(int targetRSU);
    // Data: sample the availability of registered park n rides
    void parkingLotStatus(void);
    // Format rsuData and parkingData information into protobuf
    veinsgym::proto::Request serializeObservation(void);
    // Add reward
    double calculateReward(void);
    // Reward: vehicle has exited to park and needs to schedule a reward accordingly
    void handleBufferedReward(PayloadReward *msg);

private:
    // need some sort of queue to hold RSU data
    cQueue *rsuData;
    // need some sort of queue to hold parking data
    cQueue *parkingData;
    std::vector<std::string> parkingLotList;
    cMessage *control_timer;  // Recurring self message for control updates
    GymConnection *gymCon = nullptr;
    int spawnCounter = 0;
};

}

#endif
