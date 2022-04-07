#include "BadCarApp.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ProbeVehicleData_m.h"

using namespace veins;

Define_Module(veins::BadCarApp);

void BadCarApp::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastMsgAt = simTime();
        currentSubscribedServiceId = -1;
    }
}

void BadCarApp::onWSA(DemoServiceAdvertisment* wsa)
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

void BadCarApp::onWSM(BaseFrame1609_4* frame)
{
    if ( TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(frame) ) //Responds to TraCIDemo11pMsg
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
}

void BadCarApp::handleSelfMsg(cMessage* msg)
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
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void BadCarApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    // sends message every 10s
     if (simTime() - lastMsgAt >= 10 && sentMessage == false) {
         findHost()->getDisplayString().setTagArg("i", 1, "red");
         sentMessage = true;

         ProbeVehicleData* wsm = new ProbeVehicleData();
         populateWSM(wsm);
//         wsm->setDemoData(mobility->getRoadId().c_str());

         // host is standing still due to crash
         if (dataOnSch) {
             startService(Channel::sch2, 42, "Traffic Information Service");
             // started service and server advertising, schedule message to self to send later
             scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
         }
         else {
            // send right away on CCH, because channel switching is disabled
            sendDown(wsm);
        }
        lastMsgAt = simTime();
    }
}

void BadCarApp::populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type rcvId, int serial)
{
    wsm->setRecipientAddress(rcvId);
    wsm->setBitLength(headerLength);

    if (DemoSafetyMessage* bsm = dynamic_cast<DemoSafetyMessage*>(wsm)) {
        bsm->setSenderPos(curPosition); /*Change this for spoof*/
        bsm->setSenderSpeed(curSpeed); /*Change this for spoof*/
        bsm->setPsid(-1);
        bsm->setChannelNumber(static_cast<int>(Channel::cch));
        bsm->addBitLength(beaconLengthBits);
        wsm->setUserPriority(beaconUserPriority);
    }
    else if (DemoServiceAdvertisment* wsa = dynamic_cast<DemoServiceAdvertisment*>(wsm)) {
        wsa->setChannelNumber(static_cast<int>(Channel::cch));
        wsa->setTargetChannel(static_cast<int>(currentServiceChannel));
        wsa->setPsid(currentOfferedServiceId);
        wsa->setServiceDescription(currentServiceDescription.c_str());
    }
    else if (ProbeVehicleData* pvd = dynamic_cast<ProbeVehicleData*>(wsm)) {
        /* Handle like regular WSM, but populate its data with vehicle data */
        pvd->setSenderPos(curPosition);
        pvd->setSenderSpeed(curSpeed);
        pvd->setVehicleTypeID("Default_Car");
        if (dataOnSch)
            pvd->setChannelNumber(static_cast<int>(Channel::sch1)); // will be rewritten at Mac1609_4 to actual Service Channel. This is just so no controlInfo is needed
        else
            pvd->setChannelNumber(static_cast<int>(Channel::cch));
        pvd->addBitLength(dataLengthBits);
        pvd->setUserPriority(dataUserPriority);
    }
    else {
        if (dataOnSch)
            wsm->setChannelNumber(static_cast<int>(Channel::sch1)); // will be rewritten at Mac1609_4 to actual Service Channel. This is just so no controlInfo is needed
        else
            wsm->setChannelNumber(static_cast<int>(Channel::cch));
        wsm->addBitLength(dataLengthBits);
        wsm->setUserPriority(dataUserPriority);
    }
}
