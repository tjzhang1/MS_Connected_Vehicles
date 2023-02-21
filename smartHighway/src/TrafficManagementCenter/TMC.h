#ifndef TMC_H_
#define TMC_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "protobuf/veinsgym.pb.h"
#include "VeinsGym/GymConnection.h"
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
#define THROUGHPUT_WEIGHT 1.0/600.0  // 1 car per second for 600s = 1/600
#define TARGET_TIME 400.0   // in seconds, how long it is expected for a mainline vehicle to get to the end
#define DELAY_WEIGHT -1.0/TARGET_TIME
#define CO2_WEIGHT -4.0/10000000  // magnitude of values is 10e6

/* Object that holds state info for parking lots */
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

/* Object that holds state info for RSUs */
class RsuData : public cObject {
public:
    RsuData(int id, double occ, double speed, int haltingVeh, int veh) {
        rsuId = id;
        lastStepOccupancy = occ;
        lastStepMeanSpeed = speed;
        lastStepHaltingVehiclesNumber = haltingVeh;
        vehiclesNumber = veh;
    }
    RsuData(void) {
        rsuId = 0;
        lastStepOccupancy = 0.0;
        lastStepMeanSpeed = 0.0;
        lastStepHaltingVehiclesNumber = 0;
        vehiclesNumber = 0;
    }
    int rsuId;
    double lastStepOccupancy;
    double lastStepMeanSpeed;
    int lastStepHaltingVehiclesNumber;
    int vehiclesNumber;
};

/* Defines rewards structure for ML training */
typedef struct {
    int hwyThroughput;
    simtime_t accumTravelTime;
    double accumCO2Emissions;
} rewards_t;

/* Class to hold rewards value for various vehicles */
class RewardsBuffer : public cObject {
public:
    RewardsBuffer(int throughput=0, simtime_t time=SimTime::ZERO, double emissions=0) {
        buffer.hwyThroughput = throughput;
        buffer.accumTravelTime = time;
        buffer.accumCO2Emissions = emissions;
    }
    RewardsBuffer(RewardsBuffer &r) {
        buffer = r.buffer;
        sourceIdList = r.sourceIdList;
    }
    rewards_t buffer;
    void reset(void) {
        buffer = {0,SimTime::ZERO,0};
        sourceIdList.clear();
    }
    std::vector<std::string> sourceIdList;  // List of vehicles that this buffer represents
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
    std::vector<std::string> LOG;
    rewards_t globalReward = {0,SimTime::ZERO,0};
    RewardsBuffer bufferedHOVReward;
    RewardsBuffer bufferedVehReward;
    int parkingSpaces = 400;
    const int actionRSU[5] = {
        0,  // I605_EXIT
        3,  // I605_SB_ENTER
        4,  // LAKEWOOD_EXIT
        6,  // LAKEWOOD_SB_ENTER
        7,  // PARAMOUNT_EXIT
    };
    // Data: enqueue observations from RSU and schedule processing
    void appendObs(RsuData *r);

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
