#include "CarApp.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ParkingReroute_m.h"
#include <random>
#include "Messaging/PayloadReward_m.h"

using namespace veins;

Define_Module(veins::CarApp);

void CarApp::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        spawnTime = simTime();
        TMC_connection = dynamic_cast<TMC *>(getParentModule()->getParentModule()->getSubmodule("TMC"));

        std::string vType = traciVehicle->getTypeId();
        // Setup payload rewards and determine if vehicle data should be recorded when exiting
        if(vType.compare("HOV") == 0) {
            payloadReward = TMC_connection->bufferedHOVReward;
            TMC_connection->bufferedHOVReward = {0};
            mainline_veh = true;
        }
        else if(vType.compare("continuingVeh") == 0) {
            payloadReward = TMC_connection->bufferedVehReward;
            TMC_connection->bufferedVehReward = {0};
            mainline_veh = true;
        }
        // If type is DEFAULT_VEH but spawns at mainline source
        if(traciVehicle->getRoadId().compare("ne0_source") == 0) {
            mainline_veh = true;
        }
    }
}

/* Called when vehicle exits the simulation, will be used to update global rewards */
void CarApp::finish() {
    std::cout << "finish" << endl;
    // traciVehicle declared in DemoBaseApplLayer.h
    // Handle end of mainline case - reaches end of mainline, is a mainline vehicle
    if(traciVehicle->getRoadId().compare("destination") == 0 && mainline_veh) {
        TMC_connection->globalReward.hwyThroughput++;
        TMC_connection->globalReward.accumTravelTime+=(simTime() - spawnTime);
        TMC_connection->globalReward.accumCO2Emissions+=traciVehicle->getCO2Emissions();
        // Add additional payload
        TMC_connection->globalReward.hwyThroughput += payloadReward.hwyThroughput;
        TMC_connection->globalReward.accumTravelTime += payloadReward.accumTravelTime;
        TMC_connection->globalReward.accumCO2Emissions += payloadReward.accumCO2Emissions;
    }
    // If vehicle leaves to park and there's a spot available, add values to buffered reward to be delivered as a payload for the next HOV
    else if(exitNo != UNASSIGNED && TMC_connection->parkingSpaces > 0) {
        simtime_t arrival_time = (simTime() + getTravelTime(exitNo) + SimTime(normal(0.0, 30.0)));
        simtime_t wait_time = SimTime(fmod(arrival_time.dbl(), 15.0));  // based on HOV interval
//        TMC_connection->bufferedHOVReward.hwyThroughput++;
        PayloadReward *msg = new PayloadReward("HOV_reward", TMC_BUFFERED_RWD_MSG);
        // Populate msg with this vehicle's rewards
        msg->setVType(HOV);
        msg->setTravelTime(arrival_time - spawnTime + wait_time);
        msg->setCO2Emissions(traciVehicle->getCO2Emissions());
        // Reserve parking space
        TMC_connection->parkingSpaces--;
        // Schedule a message to update the rewards payload in the future
        // RSUExampleScenario -> node -> appl
        //                   |-> TMC
        cModule *target = getParentModule()->getParentModule()->getSubmodule("TMC");
        sendDirect(msg, arrival_time - simTime(), 0, target, "RSU_port");
    }
    // If vehicle leaves to park and there's no spots left, add a vehicle at the next spawn point
    else if(exitNo != UNASSIGNED && TMC_connection->parkingSpaces == 0) {
        simtime_t return_time = (simTime() + getTravelTime(exitNo) + SimTime(normal(0.0, 60.0)) + getTravelTime(exitNo));
        PayloadReward *msg = new PayloadReward("Continuing_VEH_reward", TMC_BUFFERED_RWD_MSG);
        msg->setVType(CONTINUING_VEH);
        msg->setTravelTime(return_time - spawnTime);
        msg->setCO2Emissions(traciVehicle->getCO2Emissions());
        // Schedule a message to spawn a vehicle in the future
        cModule *target = getParentModule()->getParentModule()->getSubmodule("TMC");
        sendDirect(msg, return_time - simTime(), 0, target, "RSU_port");
    }
    DemoBaseApplLayer::finish();
}

void CarApp::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void CarApp::onWSM(BaseFrame1609_4* frame)
{
    if ( TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(frame) )
    {
        findHost()->getDisplayString().setTagArg("i", 1, "green");

        if (mobility->getRoadId()[0] != ':') traciVehicle->changeRoute(wsm->getDemoData(), 9999);
        if (!sentMessage) {
            sentMessage = true;
            // repeat the received traffic update once in 2 seconds plus some random delay
            wsm->setSenderAddress(myId);
            wsm->setSerial(3);
            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
        }
    }
    // Can add more functionality here
    else if(ParkingReroute *wsm = dynamic_cast<ParkingReroute*>(frame)) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist100(1,100); // distribution in range [1, 6]
        if(exitNo == UNASSIGNED && dist100(rng) > 5 && traciVehicle->getPlannedRoadIds().back().compare("destination")==0) {
//            for(int k=0; k<wsm->getOpenLotArraySize(); k++) {
//                const char *lotId = wsm->getOpenLot(k);
//                try {
//                    traciVehicle->changeTarget(std::string(lotId) + std::string("_dest_edge"));  // edge ID here (not lane ID)
//                    try {
//                        traciVehicle->stopAt(lotId, 70, 0, (simtime_t)5, (uint8_t)64);
//                    }
//                    catch(cRuntimeError &e) {
//                        std::cerr << e.what() << endl;
//                    }
//                }
//                catch(cRuntimeError &e){
//                    std::cerr << e.what() << endl;
//                }
//            }
            char exitCode[2] = {traciVehicle->getLaneId()[2], '\0'};
            exitNo = strtol(exitCode, NULL, 16);
            try {
                traciVehicle->changeTarget(getExit(exitNo));
            }
            catch(cRuntimeError &e){
                std::cerr << e.what() << endl;
            }
        }
    }
}

void CarApp::handleSelfMsg(cMessage* msg)
{
    if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {
        // send this message on the service channel until the counter is 3 or higher.
        // this code only runs when channel switching is enabled
        sendDown(wsm->dup());
        wsm->setSerial(wsm->getSerial() + 1);
        if (wsm->getSerial() >= 3) {
            // stop service advertisements
            stopService();
            delete (wsm);
        }
        else {
            scheduleAt(simTime() + 1, wsm);
        }
    }
    else {
        switch(msg->getKind()) {
        case PARKNRIDE_MSG: {
            // Handle reroute to parking structure
            break;
        }
        default: {
            DemoBaseApplLayer::handleSelfMsg(msg);
        }
        }
    }
}

void CarApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
}

