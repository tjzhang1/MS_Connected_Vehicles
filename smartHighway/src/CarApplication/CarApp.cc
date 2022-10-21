#include "CarApp.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ParkingReroute_m.h"

using namespace veins;

Define_Module(veins::CarApp);

void CarApp::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
    }
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
        std::cerr << "rerouting" << endl;
        traciVehicle->changeTarget("onRamp");
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

