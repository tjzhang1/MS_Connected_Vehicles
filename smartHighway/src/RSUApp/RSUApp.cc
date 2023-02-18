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
#include <boost/algorithm/string.hpp>
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
        TMC_connection = dynamic_cast<TMC *>(getParentModule()->getParentModule()->getSubmodule("TMC"));
        stringListFromParam(areaDetectorList, "areaDetectors");
        resetStatistics();
        scheduleAt(simTime() + EQUILIBRIUM_PERIOD, updateTMC_msg);
        scheduleAt(simTime() + EQUILIBRIUM_PERIOD, sampleMsg);
    }
}

void RSUApp::finish() {
    DemoBaseApplLayer::finish();
}

void RSUApp::resetStatistics() {
    accum_occupancy = 0.0;
    accum_speed = 0.0;
    accum_halting_vehicles = 0;
    accum_veh_count = 0;
    samplesCount = 0;
}

void RSUApp::handleMessage(cMessage *msg) {
    switch(msg->getKind()) {
    case RSU_BROADCAST_MSG: {
        if(msg->arrivedOn("TMC_port")) {
            broadcastLifetime = UPDATE_TMC_PERIOD / BROADCAST_INTERVAL - 1;
#if RSU_VERBOSE
            std::cout<<getParentModule()->getFullName()<<" is rerouting"<<endl;
#endif
        }
        else {
            broadcastLifetime--;
        }
        ParkingReroute *broadcastMsg = new ParkingReroute("RSU: reroute advised", VEH_ADVISORY_MSG);
        DemoBaseApplLayer::populateWSM(broadcastMsg);
        sendDown(broadcastMsg);
        // Renew message
        if(broadcastLifetime > 0) {
            scheduleAt(simTime() + BROADCAST_INTERVAL, msg);
        }
        else {
            cancelAndDelete(msg);
        }
        break;
    }
    case RSU_UPDATE_TMC_MSG: {
        if(!traci) {
            TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
            traci = manager->getCommandInterface();
        }
        sendToTMC();
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
//            accum_occupancy += sensor.getLastStepOccupancy();
            // Get last stepMeanSpeed
            int vehCount = sensor.getLastStepVehicleNumber();
            totalVehCount += vehCount;
            double lastStepMeanSpeed = sensor.getLastStepMeanSpeed();
            accum_speed_local += ( (lastStepMeanSpeed < 0.0) ? 0.0 : lastStepMeanSpeed*vehCount );
            // Get halting vehicles count
//            accum_halting_vehicles += sensor.getLastStepHaltingVehiclesNumber();
        }
        accum_speed += ( (totalVehCount > 0) ? (accum_speed_local / totalVehCount) : 0.0 );
        accum_veh_count += totalVehCount;
#if RSU_VERBOSE && DATA_SUMMARY
        std::cout << "From " << getParentModule()->getFullName() << " - ";
//        std::cout << "accum_occupancy: " << accum_occupancy << ", ";
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

void RSUApp::populateData(RsuData *data) {
    data->rsuId = getParentModule()->getIndex();
/*  May reuse this code if necessary to include detailed vehicle positions
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
*/
    // Add accumulated statistics
    double areaDetectorsCount = areaDetectorList.size();  // get average across all lanes 
    if(samplesCount > 0 && areaDetectorsCount > 0.0) {  // Avoid dividing by 0
//        data->lastStepOccupancy = accum_occupancy / (samplesCount*areaDetectorsCount);
        data->lastStepMeanSpeed = accum_speed / samplesCount;
//        data->setLastStepHaltingVehiclesNumber(accum_halting_vehicles / samplesCount);
        data->vehiclesNumber = accum_veh_count / samplesCount;
    }
    else {
//        data->lastStepOccupancy = 0.0;
        data->lastStepMeanSpeed = 0.0;
//        data->setLastStepHaltingVehiclesNumber(0);
        data->vehiclesNumber = 0;
    }
#if RSU_VERBOSE && DATA_SUMMARY
    std::cout << "From " << getParentModule()->getFullName() << " - ";
//    std::cout << "(occupancy, speed, numVehicles): " << data->lastStepOccupancy << ", " << data->lastStepMeanSpeed << ", " << data->vehiclesNumber << endl;
    std::cout << "(speed, numVehicles): " << data->lastStepMeanSpeed << ", " << data->vehiclesNumber << endl;
#endif
    // Reset sampled data values after using
    resetStatistics();
}

void RSUApp::sendToTMC(void) {
    RsuData *data = new RsuData();
    populateData(data);
    TMC_connection->appendObs(data);
}

std::list<std::string> RSUApp::getVehicleIDs() {
    std::list<std::string> vehicleIDs;
    for(auto area=areaDetectorList.begin(); area!=areaDetectorList.end(); area++) {
        TraCICommandInterface::LaneAreaDetector LAD = traci->laneAreaDetector(*area);  // Acquire connection to corresponding laneAreaDetector
        vehicleIDs.merge(LAD.getLastStepVehicleIDs());  // Append its vehicle IDs into the list
    }
    return vehicleIDs;
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
