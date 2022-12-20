#include "CarApp.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ParkingReroute_m.h"
#include <random>

using namespace veins;

Define_Module(veins::CarApp);

void CarApp::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        spawnTime = simTime();
        TMC_connection = getParentModule()->getParentModule()->getSubmodule("TMC");
        if(strcmp(traciVehicle->getTypeId(), "HOV") == 0)
            payloadReward = TMC_connection->globalReward;
    }
}

/* Called when vehicle exits the simulation, will be used to update global rewards */
void CarApp::finish() {
    std::cout << "finish" << endl;
    // traciVehicle declared in DemoBaseApplLayer.h
    // Handle end of mainline case
    if(strcmp(traciVehicle->getLaneId(), "destination") == 0 || strcmp(traciVehicle->getTypeId(), "HOV") == 0 || strcmp(traciVehicle->nodeId, "continuingVeh") == 0) {
        TMC_connection->globalReward.hwyThroughput++;
        TMC_connection->globalReward.accumTravelTime+=(simTime() - spawnTime);
        TMC_connection->globalReward.accumCO2Emissions+=traciVehicle->getCO2Emissions();
        // Add additional payload
        TMC_connection->globalReward.hwyThroughput += payloadReward.hwyThroughput;
        TMC_connection->globalReward.accumTravelTime += payloadReward.accumTravelTime;
        TMC_connection->globalReward.accumCO2Emissions += payloadReward.accumCO2Emissions;
    }
    // If vehicle leaves to park and there's a spot available, add values to buffered reward to be delivered as a payload for the next HOV
    else if(strcmp(traciVehicle->getLaneId(), "parking_destination") == 0 && TMC_connection->parkingSpaces > 0) {
        TMC_connection->bufferedHOVReward.hwyThroughput++;
        TMC_connection->bufferedHOVReward.accumTravelTime+=(simTime() - spawnTime + wait_time + random);
        TMC_connection->bufferedHOVReward.accumCO2Emissions+=traciVehicle->getCO2Emissions();
        TMC_connection->parkingSpaces--;
    }
    // If vehicle leaves to park and there's no spots left, add a vehicle at the next spawn point
    else if(strcmp(traciVehicle->getLaneId(), "parking_destination") == 0 && TMC_connection->parkingSpaces == 0) {

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
        if(dist100(rng) > 5) {
            for(int k=0; k<wsm->getOpenLotArraySize(); k++) {
                const char *lotId = wsm->getOpenLot(k);
                try {
                    traciVehicle->changeTarget(std::string(lotId) + std::string("_dest_edge"));  // edge ID here (not lane ID)
                    try {
                        traciVehicle->stopAt(lotId, 70, 0, (simtime_t)5, (uint8_t)64);
                    }
                    catch(cRuntimeError &e) {
                        std::cerr << e.what() << endl;
                    }
                }
                catch(cRuntimeError &e){
                    std::cerr << e.what() << endl;
                }
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

