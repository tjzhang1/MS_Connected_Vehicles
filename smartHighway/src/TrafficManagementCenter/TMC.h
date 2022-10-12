#ifndef TMC_H_
#define TMC_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

#define RSU_DATA_MSG 0
#define TMC_TIMER_MSG 1
#define RL_PERIOD 10

class parkingLotData : public cObject {
public:
    parkingLotData(int l, int c, int o) {
        lotID = l;
        capacity = c;
        occupancy = o;
    }
    parkingLotData(void) {
        lotID = 0;
        capacity = 0;
        occupancy = 0;
    }
    int lotID;
    int capacity;
    int occupancy;
};

class vehicleData : public cObject {
public:
    int vehID;
    int posX, posY;
    int velX, velY;
};

//typedef struct parkingLotData {
//    int lotID;
//    int capacity;
//    int occupancy;
//} parkingLotData_t;
//
//typedef struct vehicleData {
//    int vehID;
//    Coord pos;
//    Coord vel;
//} vehicleData_t;

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

private:
    // need some sort of queue to hold RSU data
    cQueue *rsuData;
    // need some sort of queue to hold parking data
    cQueue *parkingData;
    int numParkingLots;
};

}

#endif
