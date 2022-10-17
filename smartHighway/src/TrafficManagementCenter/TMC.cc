#include "TMC.h"
//#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ProbeVehicleData_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include <boost/algorithm/string.hpp>

using namespace veins;

Define_Module(veins::TMC);

TMC::TMC() {
    rsuData = new cQueue("rsuData");
    parkingData = new cQueue("parkingData");
    control_timer = new cMessage("TMC_CONTROL_TIMER", TMC_TIMER_MSG);
}

TMC::~TMC() {
    delete rsuData;
    delete parkingData;
    cancelAndDelete(control_timer);
}

void TMC::initialize(int stage) {
    if(stage == 0)
    {
        // Initialize update period timer at simulation start
        scheduleAt(simTime() + RL_PERIOD, control_timer);
        stringListFromParam(parkingLotList, "parkingLots");
    }
}

void TMC::finish() {
    // Does nothing at sim end
}

// Determine the control output for the system (signal timings, broadcast to reroute)
void TMC::computeAction(void) {
    parkingData->clear();
}
// Control: update signal timing for a particular RSU
void TMC::updateSignalTiming(void) {

}
// Control: tell RSU to broadcast vehicles to reroute
void TMC::broadcastReroute(void) {

}
// Data: sample the availability of registered park n rides
void TMC::parkingLotStatus(void) {
    TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
    TraCICommandInterface *traci = manager->getCommandInterface();
    if(manager && traci) {
        for(auto lot=parkingLotList.begin(); lot!=parkingLotList.end(); lot++) {
            int occ = std::stoi(traci->getParkingOccupancy(*lot));
            int cap = std::stoi(traci->getParkingCapacity(*lot));
            parkingLotData *p = new parkingLotData(*lot, cap, occ);
            parkingData->insert(p);
#if TMC_VERBOSE
            std::cerr << "Lot " << *lot << " occupancy is " << occ << ", ";
            std::cerr << "capacity is " << cap << endl;
#endif
        }
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

        scheduleAt(simTime() + RL_PERIOD, msg);
        break;
    }
    default: {
        delete msg;
        break;
    }
    }
}

void TMC::stringListFromParam(std::vector<std::string> &list, const char *parName) {
    std::string s = par(parName);
    boost::split(list, s, boost::is_any_of(", "), boost::token_compress_on);
#if TMC_VERBOSE
    std::cout << parName << ": ";
    for(auto i = list.begin(); i!=list.end(); i++) {
        std::cout << *i << " ";
    }
    std::cout << endl;
#endif
}
