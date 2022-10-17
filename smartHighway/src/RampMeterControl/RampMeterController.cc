/*
 * RampMeterController.cc
 *
 *  Created on: Apr 7, 2022
 *      Author: tjzhang
 */

#include <RampMeterControl/RampMeterController.h>
#include <boost/algorithm/string.hpp>
#include <stdio.h>

using namespace veins;

Define_Module(veins::RampMeterController);

void RampMeterController::stringListFromParam(std::vector<std::string> &list, const char *parName) {
    std::string s = par(parName);
    boost::split(list, s, boost::is_any_of(", "), boost::token_compress_on);
#if RMC_VERBOSE
    std::cout << parName << ": ";
    for(auto i = list.begin(); i!=list.end(); i++) {
        std::cout << *i << " ";
    }
    std::cout << endl;
#endif
}

RampMeterController::RampMeterController() {
    measureMsg = new cMessage("ALINEA: measure occupancy", RMC_ALINEA_MEASURE_MSG);
    updateMsg = new cMessage("ALINEA: update meter rate", RMC_ALINEA_UPDATE_MSG);
    changePhaseMsg = new cMessage("Set ramp meter to green", RMC_SET_GREEN_MSG);
}

RampMeterController::~RampMeterController() {
    cancelAndDelete(measureMsg);
    cancelAndDelete(updateMsg);
    cancelAndDelete(changePhaseMsg);
}

void RampMeterController::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    // Since traci isn't connected at the beginning of simulation, schedule a message
    // to tell the controller to initialize itself after 0.1s
    if(stage == 0)
    {
        if (FindModule<TraCITrafficLightInterface*>::findSubModule(getParentModule())) {
            tlInterface = TraCITrafficLightInterfaceAccess().get(getParentModule());
            traci = tlInterface->getCommandInterface();
            ASSERT(traci && tlInterface);
        }
        onRampOccupancy = 0.0;
        hwyOccupancy = 0.0;
        meterFlow = 300;
        meterRate = 15.0;

        scheduleAt(simTime() + 1, measureMsg);
        scheduleAt(simTime() + updatePeriodALINEA, updateMsg);
        scheduleAt(simTime() + (int)meterRate, changePhaseMsg);

        // Initialize highwayInductorsList as a list of strings from parameter highwayInductors
        stringListFromParam(highwayInductorsList, "highwayInductors");
        // Initialize onRampInductorsList as a list of strings from parameter onRampInductors
        stringListFromParam(onRampInductorsList, "onRampInductors");
    }
}

void RampMeterController::handleSelfMsg(cMessage* msg) {
    switch(msg->getKind()) {
        case RMC_ALINEA_MEASURE_MSG: { //received init message
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
                for(auto inductorPtr = onRampInductorsList.begin(); inductorPtr!=onRampInductorsList.end(); inductorPtr++) {
                    onRampOccupancy += traci->getInductorOccupancy(*inductorPtr);
                }
                for(auto inductorPtr = highwayInductorsList.begin(); inductorPtr!=highwayInductorsList.end(); inductorPtr++) {
                    hwyOccupancy += traci->getInductorOccupancy(*inductorPtr);
                }
            }
            scheduleAt(simTime() + 1, msg);
            break;
        }
        case RMC_ALINEA_UPDATE_MSG: {
            double numOnRampLanes = onRampInductorsList.size();
            double numHwyLanes = highwayInductorsList.size();
            double delta = KR*(targetOccupancy - (hwyOccupancy / (numHwyLanes*(float)updatePeriodALINEA)));
            if (delta > 0)
                meterFlow = std::min(rMax, meterFlow+delta);
            else
                meterFlow = std::max(rMin, meterFlow+delta);
            meterRate = 3600.0 / meterFlow;  // = 60.0 s/min / (meterFlow / 60 min/hr)
#if RMC_VERBOSE
            printf("Accumulated on ramp occupancy is %f%%\n", onRampOccupancy / (numOnRampLanes*(float)updatePeriodALINEA)); //onRampOccupancy is in units [%*s]
            printf("Accumulated hwy occupancy is %f%%\n", hwyOccupancy / (numHwyLanes*(float)updatePeriodALINEA));
            printf("Metering flow = %f VPH\n", meterFlow);
            printf("Metering rate = %f seconds\n", meterRate);
#endif
            // Reset accumulators
            onRampOccupancy = 0.0;
            hwyOccupancy = 0.0;
            scheduleAt(simTime() + updatePeriodALINEA, msg);
            break;
        }
        case RMC_SET_GREEN_MSG: {
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


