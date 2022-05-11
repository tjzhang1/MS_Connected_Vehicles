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
        if (FindModule<TraCITrafficLightInterface*>::findSubModule(getParentModule())) {
            tlInterface = TraCITrafficLightInterfaceAccess().get(getParentModule());
            traci = tlInterface->getCommandInterface();
            std::cerr << "Initialized tlInterface" << endl;
        }
        cMessage *measureMsg = new cMessage("ALINEA: measure occupancy", 100);
        cMessage *updateMsg = new cMessage("ALINEA: update meter rate", 101);
        cMessage *changePhaseMsg = new cMessage("Set ramp meter to green", 102);
        scheduleAt(simTime() + 1, measureMsg);
        scheduleAt(simTime() + updatePeriodALINEA, updateMsg);
        scheduleAt(simTime() + meterRate, changePhaseMsg);
        onRampOccupancy = 0.0;
        hwyOccupancy = 0.0;
        meterFlow = 300;
        meterRate = 15.0;
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
//            std::cerr << "Received init message\n";
            if (traci)
            {
                onRampOccupancy += traci->getInductorOccupancy("e1_onRamp");
                hwyOccupancy += traci->getInductorOccupancy("e1_hwy_lane0");
                hwyOccupancy += traci->getInductorOccupancy("e1_hwy_lane1");
                hwyOccupancy += traci->getInductorOccupancy("e1_hwy_lane2");
            }
            scheduleAt(simTime() + 1, msg);
            break;
        }
        case 101: {
            printf("Accumulated on ramp occupancy is %f%%\n", onRampOccupancy / (float)updatePeriodALINEA); //onRampOccupancy is in units [%*s]
            printf("Accumulated hwy occupancy is %f%%\n", hwyOccupancy / (numHwyLanes*(float)updatePeriodALINEA));
            double delta = KR*(targetOccupancy - (hwyOccupancy / (numHwyLanes*(float)updatePeriodALINEA)));
            if (delta > 0)
                meterFlow = std::min(rMax, meterFlow+delta);
            else
                meterFlow = std::max(rMin, meterFlow+delta);
            printf("Metering flow = %f VPH\n", meterFlow);
            meterRate = 3600.0 / meterFlow;  // = 60.0 s/min / (meterFlow / 60 min/hr)
            printf("Metering rate = %f seconds\n", meterRate);
            onRampOccupancy = 0.0;
            hwyOccupancy = 0.0;
            scheduleAt(simTime() + updatePeriodALINEA, msg);
            break;
        }
        case 102: {
            //Set phase to green (index = 1)
            tlInterface->setCurrentPhaseByNr(1, true);
            //Schedule this call again
            scheduleAt(simTime() + meterRate, msg);
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
