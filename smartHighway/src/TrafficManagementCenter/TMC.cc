#include "TMC.h"
#include "RSUApp/RSUApp.h"
#include "Messaging/ProbeVehicleData_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include <boost/algorithm/string.hpp>
#include "Messaging/RSU_Data_m.h"
#include "Messaging/ParkingReroute_m.h"
#include "Messaging/UpdateMeteringRate_m.h"

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
        stringListFromParam(parkingLotList, "parkingLots");
    }
}

void TMC::finish() {
    // Does nothing at sim end
}

//double TMC::computeReward(void) {
//
//}
//
//void TMC::computeObservation(void) {
//
//}
//
//veinsgym::proto::Request TMC::serializeObservation(void) {
//
//}

// Determine the control output for the system (signal timings, broadcast to reroute)
void TMC::computeAction(void) {
    broadcastReroute(0);
    updateSignalTiming(0, 25);
    parkingData->clear();
    rsuData->clear();
}
// Control: update signal timing for a particular RSU
void TMC::updateSignalTiming(int targetRM, double meterRate) {
    UpdateMeteringRate *msg = new UpdateMeteringRate();
    msg->setMeterRate(meterRate);
    // RSUExampleScenario -> rampMeter -> appl
    //                   |-> TMC
    cGate *target = getParentModule()->getSubmodule("rampMeter", targetRM)->getSubmodule("appl")->gate("TMC_port");
#if TMC_VERBOSE
    std::cout << "Broadcasting new interval, gate " << target->getFullName() << " acquired" << endl;
    std::cout << "New rate will be " << msg->getMeterRate() << endl;
#endif
    sendDirect(msg, 1, 0, target);
}
// Control: tell RSU to broadcast vehicles to reroute
void TMC::broadcastReroute(int targetRSU) {
    int k=0;
    ParkingReroute *msg = new ParkingReroute("broadcastReroute", RSU_BROADCAST_MSG);
    msg->setOpenLotArraySize(parkingLotList.size());
    for(auto lot=parkingLotList.begin(); lot!=parkingLotList.end(); lot++) {
        msg->setOpenLot(k++, (*lot).c_str());
    }
    // RSUExampleScenario -> rsu -> appl
    //                   |-> TMC
    cGate *target = getParentModule()->getSubmodule("rsu", targetRSU)->getSubmodule("appl")->gate("TMC_port");  // submodule name must match definition in .ned
#if TMC_VERBOSE
    std::cout << "Broadcasting new route, gate " << target->getFullName() << " acquired" << endl;
    std::cout << "Size of open lot list is " << msg->getOpenLotArraySize() << endl;
#endif
    sendDirect(msg, 1, 0, target);
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
            std::cout << "Lot " << *lot << " occupancy is " << occ << ", ";
            std::cout << "capacity is " << cap << endl;
#endif
        }
    }
}

void TMC::handleMessage(cMessage *msg) {
    // Can receive messages from gate for RSU data
    // Can receive self messages as a timer for updating control plan
    switch(msg->getKind()) {
    case TMC_DATA_MSG: {
        rsuData->insert(static_cast<RSU_Data *>(msg));
        // Doesn't cause issues if multiple RSUs feed data at once
        if(control_timer->isScheduled() == false) {
            scheduleAt(simTime() + RL_INTERVAL, control_timer);
        }
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
