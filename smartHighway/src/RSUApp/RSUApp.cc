//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "RSUApp.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/RSU_Data_m.h"
#include "Messaging/ProbeVehicleData_m.h"
#include "Messaging/ParkingReroute_m.h"
#include <boost/algorithm/string.hpp>
#include "TrafficManagementCenter/TMC.h"
#include "veins/base/utils/Coord.h"

using namespace veins;

Define_Module(veins::RSUApp);

RSUApp::RSUApp() {
    updateTMC_msg = new cMessage("RSU: send data to TMC", RSU_UPDATE_TMC_MSG);
    sampleMsg = new cMessage("RSU: accumulate data", RSU_SAMPLE_MSG);
}

RSUApp::~RSUApp() {
    cancelAndDelete(updateTMC_msg);
    cancelAndDelete(sampleMsg);
}

void RSUApp::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if(stage == 0) {
        traci = NULL;
        stringListFromParam(areaDetectorList, "areaDetectors");
        resetStatistics();
        scheduleAt(simTime() + UPDATE_TMC_PERIOD, updateTMC_msg);  // Instead of using fixed time, use a poisson process to determine next time?
        scheduleAt(simTime() + ACCUM_DATA_PERIOD, sampleMsg);
    }
}

void RSUApp::finish() {
    DemoBaseApplLayer::finish();
}

void RSUApp::resetStatistics() {
    accum_occupancy = 0.0;
    accum_speed = 0.0;
    accum_halting_vehicles = 0;
    samplesCount = 0;
}

void RSUApp::handleMessage(cMessage *msg) {
    switch(msg->getKind()) {
    case RSU_BROADCAST_MSG: {
        ParkingReroute *wsm = dynamic_cast<ParkingReroute*>(msg);
        DemoBaseApplLayer::populateWSM(wsm);
        sendDown(wsm);
        break;
    }
    case RSU_UPDATE_TMC_MSG: {
        if(!traci) {
            TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
            traci = manager->getCommandInterface();
        }
        std::list<std::string> vehicleIDs = getVehicleIDs();
        sendToTMC(vehicleIDs);
        scheduleAt(simTime() + UPDATE_TMC_PERIOD, msg);
        break;
    }
    case RSU_SAMPLE_MSG: {
        if(!traci) {
            TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
            traci = manager->getCommandInterface();
        }
        int totalVehCount = 0;
        double accum_speed_local = 0.0;
        // Loop through all lanes and accumulate data
        for(auto area=areaDetectorList.begin(); area!=areaDetectorList.end(); area++) {
            TraCICommandInterface::LaneAreaDetector sensor = traci->laneAreaDetector(*area);
            // Get last step occupancy
            accum_occupancy += sensor.getLastStepOccupancy();
            // Get last stepMeanSpeed
            int vehCount = sensor.getLastStepVehicleNumber();
            totalVehCount += vehCount;
            double lastStepMeanSpeed = sensor.getLastStepMeanSpeed();
            accum_speed_local += ( (lastStepMeanSpeed < 0.0) ? 0 : lastStepMeanSpeed*vehCount );
            // Get halting vehicles count
            accum_halting_vehicles += sensor.getLastStepHaltingVehiclesNumber();
        }
        accum_speed += ( (totalVehCount > 0) ? (accum_speed_local / totalVehCount) : 0 );
#if RSU_VERBOSE && DATA_SUMMARY
        std::cout << "From " << getParentModule()->getFullName() << " - ";
        std::cout << "accum_occupancy: " << accum_occupancy << ", ";
        std::cout << "sample[" << samplesCount << "] sampled_speed: " << ( (totalVehCount > 0) ? (accum_speed_local / totalVehCount) : 0 ) << ", ";
        std::cout << "halting_vehicles: " << accum_halting_vehicles << endl;
#endif
        samplesCount++;
        scheduleAt(simTime() + ACCUM_DATA_PERIOD, msg);
        break; 
    }
    default:
        DemoBaseApplLayer::handleMessage(msg);
    }
}

void RSUApp::populateData(RSU_Data *data, std::list<std::string> &vehicleIDs) {
    data->setRsuId(getParentModule()->getFullName());
#if RSU_VERBOSE
//    std::cout << "From " << getParentModule()->getFullName() << " - ";
//    std::cout << "Recorded vehicle IDs: ";
#endif
    // Allocate enough space for vehicles
    data->setVehiclesArraySize(vehicleIDs.size());
    // Iterate through the vehicle list
    int k = 0;
    for(auto veh=vehicleIDs.begin(); veh!=vehicleIDs.end(); veh++) {
        TraCICommandInterface::Vehicle vehicleConnection = traci->vehicle(*veh);
        VehicleData vehicle;
        vehicle.vehicleId = (omnetpp::opp_string)*veh;
        vehicle.vehicleTypeId = vehicleConnection.getVType();
        vehicle.speed = vehicleConnection.getSpeed();
        Coord pos = vehicleConnection.getPosition();
        vehicle.position[0] = pos.x;
        vehicle.position[1] = pos.y;
        data->setVehicles(k++, vehicle);  // Insert vehicle at position k
#if RSU_VERBOSE
//        std::cout << *veh << " ";
#endif
    }
#if RSU_VERBOSE
//    std::cout << endl;
#endif
    // Add accumulated statistics
    double areaDetectorsCount = areaDetectorList.size();  // get average across all lanes 
    data->setLastStepOccupancy(accum_occupancy / (samplesCount*areaDetectorsCount));
    data->setLastStepMeanSpeed(accum_speed / samplesCount);
    data->setLastStepHaltingVehiclesNumber(accum_halting_vehicles / samplesCount);
#if RSU_VERBOSE && DATA_SUMMARY
    std::cout << "From " << getParentModule()->getFullName() << " - ";
    std::cout << "(occupancy, speed, haltingVehicles): " << accum_occupancy / (samplesCount*areaDetectorsCount) << ", " << accum_speed / (samplesCount) << ", " << accum_halting_vehicles / samplesCount << endl;
#endif
    // Reset sampled data values after using
    resetStatistics();
}

void RSUApp::sendToTMC(std::list<std::string> &vehicleIDs) {
    RSU_Data *data = new RSU_Data("Collected vehicle data", TMC_DATA_MSG);  // inherited from cMessage class
    populateData(data, vehicleIDs);
    // RSUExampleScenario -> RSU -> RSUApp
    //                   |-> TMC
    cModule *target = getParentModule()->getParentModule()->getSubmodule("TMC");
    sendDirect(data, target, "RSU_port");
}

std::list<std::string> RSUApp::getVehicleIDs() {
    std::list<std::string> vehicleIDs;
    for(auto area=areaDetectorList.begin(); area!=areaDetectorList.end(); area++) {
        TraCICommandInterface::LaneAreaDetector LAD = traci->laneAreaDetector(*area);  // Acquire connection to corresponding laneAreaDetector
        vehicleIDs.merge(LAD.getLastStepVehicleIDs());  // Append its vehicle IDs into the list
    }
    return vehicleIDs;
}

void RSUApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

// Handles what happens when receives message from a lower network layer
void RSUApp::onWSM(BaseFrame1609_4* frame)
{
    if ( TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(frame) )
    {
        // this rsu repeats the received traffic update in 2 seconds plus some random delay
        sendDelayedDown(wsm->dup(), 2 + uniform(0.01, 0.2));
    }
    else if ( ProbeVehicleData* pvd = dynamic_cast<ProbeVehicleData*>(frame) )
    {
        std::cout << "Received vType " << pvd->getVehicleTypeID() << " data from pos" << pvd->getSenderPos() << ", speed" << pvd->getSenderSpeed() << std::endl;
    }
    // Can add more functionality here
}

void RSUApp::stringListFromParam(std::vector<std::string> &list, const char *parName) {
    std::string s = par(parName);
    boost::split(list, s, boost::is_any_of(", "), boost::token_compress_on);
#if RSU_VERBOSE
    std::cout << getParentModule()->getFullName() << " " << parName << ": ";
    for(auto i = list.begin(); i!=list.end(); i++) {
        std::cout << *i << " ";
    }
    std::cout << endl;
#endif
}
