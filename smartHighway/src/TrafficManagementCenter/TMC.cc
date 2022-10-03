#include "TMC.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ProbeVehicleData_m.h"

using namespace veins;

Define_Module(veins::TMC);

void TMC::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void TMC::onWSM(BaseFrame1609_4* frame)
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
