#include "TMC.h"
#include "RSUApp/RSUApp.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include <boost/algorithm/string.hpp>

using namespace veins;

Define_Module(veins::TMC);

// Make sure the RSU messages are sorted for observation
int rsuDataCompare(cObject *a, cObject *b) {
    RSU_Data *a_converted = dynamic_cast<RSU_Data *>(a);
    RSU_Data *b_converted = dynamic_cast<RSU_Data *>(b);
    return a_converted->getRsuId() - b_converted->getRsuId();
}
// Make sure the parking info are sorted for observation
int parkingCompare(cObject *a, cObject *b) {
    parkingLotData *a_converted = dynamic_cast<parkingLotData *>(a);
    parkingLotData *b_converted = dynamic_cast<parkingLotData *>(b);
    return std::strcmp(a_converted->lotID.c_str(), b_converted->lotID.c_str());
}

TMC::TMC() {
    rsuData = new cQueue("rsuData", &rsuDataCompare);
    parkingData = new cQueue("parkingData", &parkingCompare);
    control_timer = new cMessage("TMC_CONTROL_TIMER", TMC_TIMER_MSG);
}

TMC::~TMC() {
    delete rsuData;
    delete parkingData;
    cancelAndDelete(control_timer);
    if (gymCon) {
        veinsgym::proto::Request request;
        request.set_id(1);
        *(request.mutable_shutdown()) = {};
        auto response = gymCon->communicate(request);
    }
}

void TMC::initialize(int stage) {
    if(stage == 0)
    {
        stringListFromParam(parkingLotList, "parkingLots");
        gymCon = veins::FindModule<GymConnection*>::findGlobalModule();
        ASSERT(gymCon);
    }
}

void TMC::finish() {
    // Does nothing at sim end
}

veinsgym::proto::Request TMC::serializeObservation(void) {
    veinsgym::proto::Request request;
    request.set_id(12);
    // Add observation
    auto *observation_space = request.mutable_step()->mutable_observation()->mutable_tuple();
    // Insert the parking space data and calculate the corresponding reward
    observation_space->add_values()->mutable_discrete()->set_value(parkingSpaces);
/* May include again if simulating parking structures beyond just spaces
//    double parking_penalty = 0.0;
//    for(cQueue::Iterator iter=cQueue::Iterator(*parkingData); iter.end()==false; iter++) {
//        auto *data = dynamic_cast<parkingLotData *>(*iter);
//        // Add a space to observation tuple and create a box there
//        auto *data_box = observation_space->add_values()->mutable_discrete();
//        int occ = data->occupancy;
//        double cap = data->capacity;
//        data_box->set_value(occ);
//        // accumulate rewards
//        parking_penalty += (occ/cap);
//    }
//   double parking_penalty *= PARKING_PENALTY_WEIGHT;
 */
    for(cQueue::Iterator iter=cQueue::Iterator(*rsuData); iter.end()==false; iter++) {
        auto *data = dynamic_cast<RSU_Data *>(*iter);
        // Add a space to observation tuple and create a box there
        auto *data_box = observation_space->add_values()->mutable_box();
        data_box->add_values(data->getLastStepOccupancy());
        data_box->add_values(data->getLastStepMeanSpeed());
        data_box->add_values((double)data->getVehiclesNumber());
    }
    // Add reward
    request.mutable_step()->mutable_reward()->mutable_box()->add_values(calculateReward());
    return request;
}

double TMC::calculateReward() {
    double reward = 0.0;
    reward += THROUGHPUT_WEIGHT * globalReward.hwyThroughput;
    reward += DELAY_WEIGHT * globalReward.accumTravelTime.dbl();
    reward += CO2_WEIGHT * globalReward.accumCO2Emissions;
    // Reset reward after reporting to ML script
    globalReward = {0,SimTime::ZERO,0};
    return reward;
}

// Determine the control output for the system (signal timings, broadcast to reroute)
void TMC::computeAction(void) {
    veinsgym::proto::Request request = serializeObservation();
    veinsgym::proto::Reply response = gymCon->communicate(request);
    auto broadcast_switch = response.mutable_action();
/* May include again if need to adjust ramp meter timings
//    auto rmc_timings = response.mutable_action()->mutable_tuple()->values(0);
//    auto broadcast_switch = response.mutable_action()->mutable_tuple()->values(1);
//    for(int i=0; i<rmc_timings.mutable_box()->values_size(); i++) {
//        updateSignalTiming(i, rmc_timings.mutable_box()->values(i));
//    }
 */
    // tell RSUs to broadcast
    for(int i=0; i<broadcast_switch->mutable_multi_binary()->values_size(); i++) {
        if(broadcast_switch->mutable_multi_binary()->values(i) == true) {
            broadcastReroute(i);
        }
    }
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
    std::cout << "Broadcasting new interval to " << target->getOwnerModule()->getParentModule()->getFullName() << ", ";
    std::cout << "new rate will be " << msg->getMeterRate() << endl;
#endif
    sendDirect(msg, 1, 0, target);
}
// Control: tell RSU to broadcast vehicles to reroute
void TMC::broadcastReroute(int targetRSU) {
    cMessage *msg = new cMessage("broadcastRerouteBeacon", RSU_BROADCAST_MSG);
/* May include again if simulating parking structures in SUMO
//    int k=0;
//    msg->setOpenLotArraySize(parkingLotList.size());
//    for(auto lot=parkingLotList.begin(); lot!=parkingLotList.end(); lot++) {
//        msg->setOpenLot(k++, (*lot).c_str());
//    }
*/
    // RSUExampleScenario -> rsu -> appl
    //                   |-> TMC
    cGate *target = getParentModule()->getSubmodule("rsu", targetRSU)->getSubmodule("appl")->gate("TMC_port");  // submodule name must match definition in .ned
#if TMC_VERBOSE
    std::cout << "Broadcasting new route to " << target->getOwnerModule()->getParentModule()->getFullName() << ", ";
    std::cout << "size of open lot list is " << msg->getOpenLotArraySize() << endl;
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

void TMC::handleBufferedReward(PayloadReward *msg) {
    if(msg->getVType() == HOV) {
        bufferedHOVReward.hwyThroughput++;
        bufferedHOVReward.accumTravelTime+=msg->getTravelTime();
        bufferedHOVReward.accumCO2Emissions+=msg->getCO2Emissions();
    }
    else {
        // Spawn vehicle with extra characteristics
        TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
        TraCICommandInterface *traci = manager->getCommandInterface();
        traci->addVehicle("continuingVehicle" + std::to_string(spawnCounter++), "continuingVehicle", "continuingRoute", simTime());
        bufferedVehReward.hwyThroughput++;
        bufferedVehReward.accumTravelTime+=msg->getTravelTime();
        bufferedVehReward.accumCO2Emissions+=msg->getCO2Emissions();
//        std::cout<<"spawning new continuing vehicle"<<endl;
    }
}

void TMC::handleMessage(cMessage *msg) {
    // Can receive messages from gate for RSU data
    // Can receive self messages as a timer for updating control plan
    // Can receive messages from vehicles to assign payload rewards
    switch(msg->getKind()) {
    case TMC_DATA_MSG: {
        rsuData->insert(dynamic_cast<RSU_Data *>(msg));
        // Doesn't cause issues if multiple RSUs feed data at once
        if(control_timer->isScheduled() == false) {
            scheduleAt(simTime() + RL_INTERVAL, control_timer);
        }
        break;
    }
    case TMC_TIMER_MSG: {
        computeAction();
        break;
    }
    case TMC_BUFFERED_RWD_MSG: {
        handleBufferedReward(dynamic_cast<PayloadReward *>(msg));
        delete msg;
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
