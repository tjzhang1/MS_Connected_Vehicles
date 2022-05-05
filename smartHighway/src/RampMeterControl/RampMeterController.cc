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
        cMessage *initMsg = new cMessage("ramp meter init", 100);
        cMessage *updateMsg = new cMessage("update ALINEA", 101);
        scheduleAt(simTime() + 1, initMsg);
        scheduleAt(simTime() + updatePeriodALINEA, updateMsg);
        onRampOccupancy = 0.0;
        hwyOccupancy = 0.0;
        meterFlow = 300;
        meterRate = 10.0;
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
//                printf("On ramp occupancy is %.4f%%\n", occupancy);
                onRampOccupancy += traci->getInductorOccupancy("e1_onRamp");
                hwyOccupancy += traci->getInductorOccupancy("e1_hwy_lane0");
                hwyOccupancy += traci->getInductorOccupancy("e1_hwy_lane1");
                hwyOccupancy += traci->getInductorOccupancy("e1_hwy_lane2");
//                std::cerr << "last step occupancy: " << occupancy << endl;
//                std::list<std::string> inductionLoops = traci->getInductorIds();
//                for (std::string loopID : inductionLoops) //LOOP THROUGH LIST OF STRINGS
//                {
//                    std::cout << "induction loop: " << loopID << endl;
//                    double occupancy;
//                    occupancy = traci->getInductorOccupancy(loopID); // not returning requested data for some reason
//                    printf("On ramp occupancy is %.4f%%\n", occupancy);
//                }
            }
            scheduleAt(simTime() + 1, msg);
            break;
        }
        case 101: {
            std::cerr << "Received update msg\n";
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
        default:
            DemoBaseApplLayer::handleSelfMsg(msg);
            break;
    }
}

void RampMeterController::onWSM(BaseFrame1609_4* wsm) {

}

void RampMeterController::onWSA(DemoServiceAdvertisment* wsa) {

}
