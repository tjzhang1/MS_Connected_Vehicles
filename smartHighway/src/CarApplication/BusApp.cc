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

#include <CarApplication/BusApp.h>

using namespace veins;

Define_Module(veins::BusApp);

BusApp::BusApp() {
    // TODO Auto-generated constructor stub
}

BusApp::~BusApp() {
    // TODO Auto-generated destructor stub
}

void BusApp::initialize(int stage) {
    BasicCarApp::initialize(stage);
    if(stage == 0) {
        spawnTime = simTime();
        TMC_connection = dynamic_cast<TMC *>(getParentModule()->getParentModule()->getSubmodule("TMC"));
        // Collect contents of HOV reward buffer
        payloadReward = TMC_connection->bufferedHOVReward;
        // Reset HOV reward buffer
        TMC_connection->bufferedHOVReward = {0,SimTime::ZERO,0};
    }
}

void BusApp::finish() {
    BasicCarApp::finish();
    TMC_connection->globalReward.hwyThroughput++;
    TMC_connection->globalReward.accumTravelTime+=(simTime() - spawnTime);
    TMC_connection->globalReward.accumCO2Emissions+=CO2Emissions;
    // Add additional payload
    TMC_connection->globalReward.hwyThroughput += payloadReward.hwyThroughput;
    TMC_connection->globalReward.accumTravelTime += payloadReward.accumTravelTime;
    TMC_connection->globalReward.accumCO2Emissions += payloadReward.accumCO2Emissions;
#if BUSAPP_VERBOSE
    std::cout<<"Exited a bus with travel time " << (simTime()-spawnTime).str() << "s and emissions " << CO2Emissions <<"\t";
    std::cout<<"THROUGHPUT: "<<TMC_connection->globalReward.hwyThroughput<<", "<<"TIME: "<<TMC_connection->globalReward.accumTravelTime.str()<<", "<<"Emissions: "<<TMC_connection->globalReward.accumCO2Emissions<<endl;
#endif
}

void BusApp::handlePositionUpdate(cObject* obj) {
    CO2Emissions += traciVehicle->getCO2Emissions();
#if BUSAPP_VERY_VERBOSE
    std::cout<<getParentModule()->getFullName()<<" co2: "<<CO2Emissions<<" mg"<<endl;
#endif
}

