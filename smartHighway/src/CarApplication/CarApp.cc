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

        vType = traciVehicle->getTypeId();
        // Setup payload rewards and determine if vehicle data should be recorded when exiting
        if(vType.compare("pt_bus") == 0) {
            payloadReward = TMC_connection->bufferedHOVReward;
            TMC_connection->bufferedHOVReward = {0,SimTime::ZERO,0};
            mainline_veh = true;
//            std::cout << "Entered HOV" << getParentModule()->getFullName() << endl;
        }
        else if(vType.compare("continuingVehicle") == 0) {
            payloadReward = TMC_connection->bufferedVehReward;
            TMC_connection->bufferedVehReward = {0,SimTime::ZERO,0};
            mainline_veh = true;
//            std::cout << "Entered continuing vehicle" << endl;
        }
        // If type is DEFAULT_VEH but spawns at mainline source
        if(traciVehicle->getRoadId().compare("ne0_source") == 0) {
            mainline_veh = true;
        }
    }
}

/* Called when vehicle exits the simulation, will be used to update global rewards */
void CarApp::finish() {
    // traciVehicle declared in DemoBaseApplLayer.h
    // Handle end of mainline case - reaches end of mainline, is a mainline vehicle
    if(currentRoad.compare("destination_edge") == 0 && mainline_veh) {
        TMC_connection->globalReward.hwyThroughput++;
        TMC_connection->globalReward.accumTravelTime+=(simTime() - spawnTime);
        TMC_connection->globalReward.accumCO2Emissions+=CO2Emissions;
        // Add additional payload
        TMC_connection->globalReward.hwyThroughput += payloadReward.hwyThroughput;
        TMC_connection->globalReward.accumTravelTime += payloadReward.accumTravelTime;
        TMC_connection->globalReward.accumCO2Emissions += payloadReward.accumCO2Emissions;
        std::cout<<"Exited a "<<vType<<" with travel time " << (simTime()-spawnTime).str() << "s and emissions " << CO2Emissions <<"\t";
        std::cout<<"THROUGHPUT: "<<TMC_connection->globalReward.hwyThroughput<<", "<<"TIME: "<<TMC_connection->globalReward.accumTravelTime.str()<<", "<<"Emissions: "<<TMC_connection->globalReward.accumCO2Emissions<<endl;
    }
    // If vehicle leaves to park and there's a spot available, add values to buffered reward to be delivered as a payload for the next HOV
    else if(exitNo != UNASSIGNED && TMC_connection->parkingSpaces > 0) {
//        simtime_t arrival_time = (simTime() + getTravelTime(exitNo) + SimTime(normal(0.0, 30.0)));
//        simtime_t wait_time = SimTime(fmod(arrival_time.dbl(), 15.0));  // based on HOV interval
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
    else if(exitNo != UNASSIGNED && TMC_connection->parkingSpaces == 0) {
        double travel_time = abs( normal(2*getTravelTime(exitNo), getTravelTime(exitNo)/4) );
//        simtime_t return_time = (simTime() + getTravelTime(exitNo) + SimTime(normal(0.0, getTravelTime(exitNo).dbl()/4.0)) + getTravelTime(exitNo));
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

void CarApp::redirect(void) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist100(1,100); // distribution in range [1, 6]
    if(exitNo == UNASSIGNED && vType.compare("DEFAULT_VEHTYPE") == 0 && dist100(rng) < 5 && traciVehicle->getPlannedRoadIds().back().compare("destination_edge")==0) {
        char exitCode[2] = {currentRoad[2], '\0'};
        exitNo = strtol(exitCode, NULL, 16);
//            std::cout << "Rerouting vehicle " << getParentModule()->getFullName();
//            std::cout << " to exit " << exitCode;
//            std::cout << " = " << getExit(exitNo) << endl;
        try {
            traciVehicle->changeTarget(getExit(exitNo));
        }
        catch(cRuntimeError &e){
            std::cerr << e.what() << endl;
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
        redirect();
//        std::random_device dev;
//        std::mt19937 rng(dev());
//        std::uniform_int_distribution<std::mt19937::result_type> dist100(1,100); // distribution in range [1, 6]
//        if(dist100(rng) > 5) {
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
//        }
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
    currentRoad = traciVehicle->getRoadId();
    CO2Emissions += traciVehicle->getCO2Emissions();
//    std::string result;
//    traciVehicle->getParameter("device.emissions.CO2_abs", result);
//    std::cout<<result<<endl;
    DemoBaseApplLayer::handlePositionUpdate(obj);
}

