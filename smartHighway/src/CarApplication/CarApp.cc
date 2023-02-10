#include "CarApp.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ParkingReroute_m.h"
#include "Messaging/PayloadReward_m.h"

using namespace veins;

Define_Module(veins::CarApp);

void CarApp::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        spawnTime = simTime();
        TMC_connection = dynamic_cast<TMC *>(getParentModule()->getParentModule()->getSubmodule("TMC"));
    }
}

/* Called when vehicle exits the simulation, will be used to update global rewards */
void CarApp::finish() {
    // traciVehicle declared in DemoBaseApplLayer.h
    // If no exit was assigned, means we reached end of mainline
    if(exitNo == UNASSIGNED) {
        TMC_connection->globalReward.hwyThroughput++;
        TMC_connection->globalReward.accumTravelTime+=(simTime() - spawnTime);
        TMC_connection->globalReward.accumCO2Emissions+=CO2Emissions;
#if CARAPP_VERBOSE
        std::cout<<"Exited a car with travel time " << (simTime()-spawnTime).str() << "s and emissions " << CO2Emissions <<"\t";
        std::cout<<"THROUGHPUT: "<<TMC_connection->globalReward.hwyThroughput<<", "<<"TIME: "<<TMC_connection->globalReward.accumTravelTime.str()<<", "<<"Emissions: "<<TMC_connection->globalReward.accumCO2Emissions<<endl;
#endif
    }
    // If vehicle leaves to park and there's a spot available, add values to buffered reward to be delivered as a payload for the next HOV
    else if(TMC_connection->parkingSpaces > 0) {
        double travel_time = abs( normal(getTravelTime(exitNo), getTravelTime(exitNo)/4) );
        double wait_time = fmod(simTime().dbl() + travel_time, 15.0);  // based on HOV interval
        PayloadReward *msg = new PayloadReward("HOV_reward", TMC_BUFFERED_RWD_MSG);
        // Populate msg with this vehicle's rewards
        msg->setVType(HOV);
        msg->setTravelTime(SimTime(travel_time + wait_time) + simTime() - spawnTime);
        msg->setCO2Emissions(CO2Emissions);
        // Reserve parking space
        TMC_connection->parkingSpaces--;
        // Schedule a message to update the rewards payload in the future
        // RSUExampleScenario -> node -> appl
        //                   |-> TMC
        cModule *target = getParentModule()->getParentModule()->getSubmodule("TMC");
        sendDirect(msg, SimTime(travel_time + wait_time), 0, target, "RSU_port");
    }
    // If vehicle leaves to park and there's no spots left, add a vehicle at the next spawn point
    else if(TMC_connection->parkingSpaces == 0) {
        double travel_time = abs( normal(2*getTravelTime(exitNo), getTravelTime(exitNo)/4) );  // Roughly twice the time it takes to get to the PAR (perhaps should not be 2x, but time taken to PAR + reentering at nearest ramp
        PayloadReward *msg = new PayloadReward("Continuing_VEH_reward", TMC_BUFFERED_RWD_MSG);
        msg->setVType(CONTINUING_VEH);
        msg->setTravelTime(SimTime(travel_time) + simTime() - spawnTime);
        msg->setCO2Emissions(CO2Emissions);
        // Schedule a message to spawn a vehicle in the future
        cModule *target = getParentModule()->getParentModule()->getSubmodule("TMC");
        sendDirect(msg, SimTime(travel_time), 0, target, "RSU_port");
    }
    DemoBaseApplLayer::finish();
}

void CarApp::redirect(void) {
    consideredRerouting = true;
    if(exitNo == UNASSIGNED && uniform(0, 100) < 5.0) {  // generates a value in range [0,100)
        std::string currentRoad = traciVehicle->getRoadId();
        char exitCode[2] = {currentRoad[2], '\0'};
        exitNo = strtol(exitCode, NULL, 16);
#if CARAPP_VERBOSE
        std::cout << "Rerouting vehicle " << getParentModule()->getFullName();
        std::cout << " to exit " << exitCode;
        std::cout << " = " << getExit(exitNo) << endl;
#endif
        try {
            traciVehicle->changeTarget(getExit(exitNo));
        }
        catch(cRuntimeError &e){
            // An error usually occurs if the currentRoad doesn't match the actual road of the vehicle
            // The simple fix is to choose the next exit
            try{
                exitNo++;
                traciVehicle->changeTarget(getExit(exitNo));
            }
            catch(cRuntimeError &e) {
                std::cerr << e.what() << endl;
            }
        }
    }
}

void CarApp::onWSM(BaseFrame1609_4* frame)
{
    if(ParkingReroute *wsm = dynamic_cast<ParkingReroute*>(frame)) {
        redirect();
    }
}

void CarApp::handlePositionUpdate(cObject* obj)
{
    CO2Emissions += traciVehicle->getCO2Emissions();
    DemoBaseApplLayer::handlePositionUpdate(obj);
}

