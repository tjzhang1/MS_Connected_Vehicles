#include "TMC.h"
//#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ProbeVehicleData_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include <stdio.h>

using namespace veins;

Define_Module(veins::TMC);

void TMC::initialize(int stage) {
    // Acquire traci command interface
    TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
    TraCICommandInterface *traci = manager->getCommandInterface();
    if(manager)
        std::cout << "Manager init" << endl;
    if(traci)
        std::cout << "traci init" << endl;

    // Initialize update period timer at simulation start
    cMessage *timer = new cMessage("RL_PERIOD_TIMER", TMC_TIMER_MSG);
    scheduleAt(simTime() + RL_PERIOD, timer);
}

void TMC::finish() {
    // Does nothing at sim end
}

// Determine the control output for the system (signal timings, broadcast to reroute)
void TMC::computeAction(void) {

}
// Control: update signal timing for a particular RSU
void TMC::updateSignalTiming(void) {

}
// Control: tell RSU to broadcast vehicles to reroute
void TMC::broadcastReroute(void) {

}
// Data: sample the availability of registered park n rides
void TMC::parkingLotStatus(void) {
    for(int i=0; i<numParkingLots; i++) {
        parkingLotData *p = new parkingLotData();
        parkingData->insert(p);
    }
}
// Data: sample the position and velocities of cars near RSUs
void TMC::collectTrafficInfo(void) {

}

void TMC::handleMessage(cMessage *msg) {
    // Can receive messages from gate for RSU data
    // Can receive self messages as a timer for updating control plan
    switch(msg->getKind()) {
    case RSU_DATA_MSG: {
        collectTrafficInfo();
        break;
    }
    case TMC_TIMER_MSG: {
        parkingLotStatus();
        computeAction();
        break;
    }
    default: {
        delete msg;
        break;
    }
    }
}

TMC::TMC() {
    rsuData = new cQueue("rsuData");
    parkingData = new cQueue("parkingData");
    numParkingLots = 2;
}

TMC::~TMC() {
    delete rsuData;
    delete parkingData;
}
