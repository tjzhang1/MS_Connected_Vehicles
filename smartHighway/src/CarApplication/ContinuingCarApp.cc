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

#include <CarApplication/ContinuingCarApp.h>
#include <boost/algorithm/string.hpp>

using namespace veins;

Define_Module(veins::ContinuingCarApp);

ContinuingCarApp::ContinuingCarApp() {
    // TODO Auto-generated constructor stub
}

ContinuingCarApp::~ContinuingCarApp() {
    // TODO Auto-generated destructor stub
}

void ContinuingCarApp::initialize(int stage) {
    BasicCarApp::initialize(stage);
    if(stage == 0) {
        spawnTime = simTime();
        TMC_connection = dynamic_cast<TMC *>(getParentModule()->getParentModule()->getSubmodule("TMC"));
        // Collect contents of continuing vehicle reward buffer
        payloadReward = RewardsBuffer(TMC_connection->bufferedVehReward);
        // Reset continuing vehicle reward buffer
        TMC_connection->bufferedVehReward.reset();
    }
}

void ContinuingCarApp::finish() {
    BasicCarApp::finish();
    TMC_connection->globalReward.hwyThroughput++;
    TMC_connection->globalReward.accumTravelTime+=(simTime() - spawnTime);
    TMC_connection->globalReward.accumCO2Emissions+=CO2Emissions;
    // Add additional payload
    TMC_connection->globalReward.hwyThroughput += payloadReward.buffer.hwyThroughput;
    TMC_connection->globalReward.accumTravelTime += payloadReward.buffer.accumTravelTime;
    TMC_connection->globalReward.accumCO2Emissions += payloadReward.buffer.accumCO2Emissions;
    // Log list of vehicles this car represents
    payloadReward.sourceIdList.push_back("Leader:"+mobility->getExternalId());
    TMC_connection->LOG.push_back(boost::algorithm::join(payloadReward.sourceIdList, " "));
#if CONTCARAPP_VERBOSE
    std::cout<<"Exited a continuing car with travel time " << (simTime()-spawnTime+payloadReward.accumTravelTime).str() << "s and emissions " << CO2Emissions+payloadReward.accumCO2Emissions<<"\t";
    std::cout<<"THROUGHPUT: "<<TMC_connection->globalReward.hwyThroughput<<", "<<"TIME: "<<TMC_connection->globalReward.accumTravelTime.str()<<", "<<"Emissions: "<<TMC_connection->globalReward.accumCO2Emissions<<endl;
#endif
}

void ContinuingCarApp::handlePositionUpdate(cObject* obj) {
    CO2Emissions += traciVehicle->getCO2Emissions();
#if CONTCARAPP_VERY_VERBOSE
    std::cout<<getParentModule()->getFullName()<<" co2: "<<CO2Emissions<<" mg"<<endl;
#endif
}

