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
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "Messaging/ProbeVehicleData_m.h"
#include <boost/algorithm/string.hpp>
#include "TrafficManagementCenter/TMC.h"

using namespace veins;

Define_Module(veins::RSUApp);

RSUApp::RSUApp() {
    samplingMsg = new cMessage("RSU: send data to TMC", RSU_SAMPLE_MSG);
}

RSUApp::~RSUApp() {
    cancelAndDelete(samplingMsg);
}

void RSUApp::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if(stage == 0) {
        stringListFromParam(areaDetectorList, "areaDetectors");
        scheduleAt(simTime() + SAMPLING_PERIOD, samplingMsg);
    }
}

void RSUApp::finish() {
    DemoBaseApplLayer::finish();
}

void RSUApp::handleSelfMsg(cMessage *msg) {
    switch(msg->getKind()) {
    case RSU_BROADCAST_MSG: {
        break;
    }
    case RSU_SAMPLE_MSG: {
        sampleAreaDetectors();
        sendToTMC();
        scheduleAt(simTime() + SAMPLING_PERIOD, msg);
        break;
    }
    default:
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void RSUApp::sendToTMC() {
    cMessage *data = new cMessage("Hi there", TMC_DATA_MSG);  // Send the actual data
    // RSUExampleScenario -> RSU -> RSUApp
    //                   |-> TMC
    cModule *target = getParentModule()->getParentModule()->getSubmodule("TMC");
    sendDirect(data, target, "RSU_port", 0);  // How to take advantage of this index?
}

std::list<std::string> RSUApp::sampleAreaDetectors() {
    TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
    TraCICommandInterface *traci = manager->getCommandInterface();

    std::list<std::string> vehicleIDs;
    for(auto area=areaDetectorList.begin(); area!=areaDetectorList.end(); area++) {
        TraCICommandInterface::LaneAreaDetector LAD = traci->laneAreaDetector(*area);  // Acquire connection to corresponding laneAreaDetector
        vehicleIDs.merge(LAD.getLastStepVehicleIDs());  // Append its vehicle IDs into the list
    }
#if RSU_VERBOSE
    std::cout << "Recorded vehicle IDs: ";
    for(auto veh=vehicleIDs.begin(); veh!=vehicleIDs.end(); veh++) {
        std::cout << *veh << " ";
    }
    std::cout << endl;
#endif
    return vehicleIDs;
}

void RSUApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void RSUApp::onWSM(BaseFrame1609_4* frame)
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

void RSUApp::stringListFromParam(std::vector<std::string> &list, const char *parName) {
    std::string s = par(parName);
    boost::split(list, s, boost::is_any_of(", "), boost::token_compress_on);
#if RSU_VERBOSE
    std::cout << parName << ": ";
    for(auto i = list.begin(); i!=list.end(); i++) {
        std::cout << *i << " ";
    }
    std::cout << endl;
#endif
}
