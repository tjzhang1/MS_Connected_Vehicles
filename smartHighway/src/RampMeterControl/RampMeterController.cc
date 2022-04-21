/*
 * RampMeterController.cc
 *
 *  Created on: Apr 7, 2022
 *      Author: tjzhang
 */

#include <RampMeterControl/RampMeterController.h>
#include <stdio.h>

using namespace veins;

Define_Module(veins::RampMeterController);

void RampMeterController::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    // Since traci isn't connected at the beginning of simulation, schedule a message
    // to tell the controller to initialize itself after 0.1s
    if(stage == 0)
    {
        cMessage *initMsg = new cMessage("ramp meter init", 100);
        scheduleAt(simTime() + 0.1, initMsg);
    }
}
void RampMeterController::handleSelfMsg(cMessage* msg) {
    switch(msg->getKind()) {
        case 100: { //received init message
//            // Read which traffic light to control from omnetpp.ini file
//            trafficLightID = hasPar("rampMeterID") ? par("rampMeterID").stringValue() : "";
//            if(!trafficLightID.empty())
//            {
//                // obtain control of Traci commands for traffic light
//                (traci->trafficlight(trafficLightID)).setProgram("my_program");
//            }
            printf("Received init message\n");
            break;
        }
        default:
            DemoBaseApplLayer::handleSelfMsg(msg);
            break;
    }
}

void RampMeterController::onWSM(BaseFrame1609_4* wsm) {

}

void RampMeterController::onWSA(DemoServiceAdvertisment* wsa) {

}
