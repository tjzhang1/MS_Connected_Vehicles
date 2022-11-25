/*
 * RampMeterController.cc
 *
 *  Created on: Apr 7, 2022
 *      Author: tjzhang
 */

#include <RampMeterControl/RampMeterController.h>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include "Messaging/UpdateMeteringRate_m.h"

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
    changePhaseMsg = new cMessage("Set ramp meter to green", RMC_SET_GREEN_MSG);
}

RampMeterController::~RampMeterController() {
    if(controlType.compare("ALINEA") == 0) {
        cancelAndDelete(measureMsg);
        cancelAndDelete(updateMsg);
    }
    else {
       cancelAndDelete(updateTMC_msg);
       cancelAndDelete(sampleMsg);
    }
    cancelAndDelete(changePhaseMsg);
}

void RampMeterController::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    // Since traci isn't connected at the beginning of simulation, schedule a message
    // to tell the controller to initialize itself after 0.1s
    if(stage == 0)
    {
        std::string c = par("controlType");
        controlType = c;
        if(controlType.compare("ALINEA") == 0) {
            measureMsg = new cMessage("ALINEA: measure occupancy", RMC_ALINEA_MEASURE_MSG);
            updateMsg = new cMessage("ALINEA: update meter rate", RMC_ALINEA_UPDATE_MSG);
            onRampOccupancy = 0.0;
            hwyOccupancy = 0.0;
            meterFlow = 300;
            scheduleAt(simTime() + 1, measureMsg);
            scheduleAt(simTime() + updatePeriodALINEA, updateMsg);
        }
        else {
           updateTMC_msg = new cMessage("RSU: send data to TMC", RSU_UPDATE_TMC_MSG);
            sampleMsg = new cMessage("RSU: accumulate data", RSU_SAMPLE_MSG);
            stringListFromParam(areaDetectorList, "areaDetectors");
        resetStatistics();
        scheduleAt(simTime() + UPDATE_TMC_PERIOD, updateTMC_msg);  // Instead of using fixed time, use a poisson process to determine next time?
        scheduleAt(simTime() + ACCUM_DATA_PERIOD, sampleMsg);
        }
        if (FindModule<TraCITrafficLightInterface*>::findSubModule(getParentModule())) {
            tlInterface = TraCITrafficLightInterfaceAccess().get(getParentModule());
            traci = tlInterface->getCommandInterface();
            ASSERT(traci && tlInterface);
        }
        meterRate = 15.0;
        scheduleAt(simTime() + meterRate, changePhaseMsg);

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
        case RMC_RL_SAMPLE_MSG: {
        if(!traci) {
            TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
            traci = manager->getCommandInterface();
        }
        for(auto area=areaDetectorList.begin(); area!=areaDetectorList.end(); area++) {
            TraCICommandInterface::LaneAreaDetector sensor = traci->laneAreaDetector(*area);
            // Get last step occupancy
            accum_occupancy += sensor.getLastStepOccupancy();
            // Get last stepMeanSpeed
            accum_speed += sensor.getLastStepMeanSpeed();
            accum_halting_vehicles += sensor.getLastStepHaltingVehiclesNumber();
        }
        samplesCount++;
        scheduleAt(simTime() + ACCUM_DATA_PERIOD, msg);
        break; 
        }
        case RMC_RL_UPDATE_TMC_MSG: {
        if(!traci) {
            TraCIScenarioManager *manager = TraCIScenarioManagerAccess().get();
            traci = manager->getCommandInterface();
        }
        std::list<std::string> vehicleIDs = getVehicleIDs();
        sendToTMC(vehicleIDs);
        scheduleAt(simTime() + UPDATE_TMC_PERIOD, msg);
        break;
        }
        default:
            DemoBaseApplLayer::handleSelfMsg(msg);
            break;
    }
}

void RampMeterController::handleMessage(cMessage * msg) {
    if(UpdateMeteringRate *newMsg = dynamic_cast<UpdateMeteringRate*>(msg)) {
        meterRate = newMsg->getMeterRate();
        delete(msg);
    }
    else {
        DemoBaseApplLayer::handleMessage(msg);
    }
}

void RampMeterController::populateData(RSU_Data *data, std::list<std::string> &vehicleIDs) {
    data->setRsuId(getParentModule()->getFullName());
#if RMC_VERBOSE
    std::cout << "From " << getParentModule()->getFullName() << " - ";
    std::cout << "Recorded vehicle IDs: ";
#endif
    // Allocate enough space for vehicles
    data->setVehiclesArraySize(vehicleIDs.size());
    // Iterate through the vehicle list
    int k = 0;
    for(auto veh=vehicleIDs.begin(); veh!=vehicleIDs.end(); veh++) {
        TraCICommandInterface::Vehicle vehicleConnection = traci->vehicle(*veh);
        VehicleData vehicle;
        vehicle.vehicleId = (omnetpp::opp_string)*veh;
        vehicle.vehicleTypeId = vehicleConnection.getVType();
        vehicle.speed = vehicleConnection.getSpeed();
        Coord pos = vehicleConnection.getPosition();
        vehicle.position[0] = pos.x;
        vehicle.position[1] = pos.y;
        data->setVehicles(k++, vehicle);  // Insert vehicle at position k
#if RMC_VERBOSE
        std::cout << *veh << " ";
#endif
    }
#if RMC_VERBOSE
    std::cout << endl;
#endif
    // Add accumulated statistics
    double areaDetectorsCount = areaDetectorList.size();  // get average across all lanes 
    data->setLastStepOccupancy(accum_occupancy / (samplesCount*areaDetectorsCount))
    data->setLastStepMeanSpeed(accum_speed / (samplesCount*areaDetectorsCount));
    data->setLastStepHaltingVehiclesNumber(accum_halting_vehicles / samplesCount);
    // Reset sampled data values after using
    resetStatistics();
}

void RampMeterController::resetStatistics() {
    accum_occupancy = 0.0;
    accum_speed = 0.0;
    accum_halting_vehicles = 0;
    samplesCount = 0;
}

void RampMeterController::sendToTMC(std::list<std::string> &vehicleIDs) {
    RSU_Data *data = new RSU_Data("Collected vehicle data", TMC_DATA_MSG);  // inherited from cMessage class
    populateData(data, vehicleIDs);
    // RSUExampleScenario -> TrafficLight -> RampMeterController
    //                   |-> TMC
    /******** Change these ************/
    cModule *target = getParentModule()->getParentModule()->getSubmodule("TMC");
    sendDirect(data, target, "RSU_port");
}

std::list<std::string> RampMeterController::getVehicleIDs() {
    std::list<std::string> vehicleIDs;
    for(auto area=areaDetectorList.begin(); area!=areaDetectorList.end(); area++) {
        TraCICommandInterface::LaneAreaDetector LAD = traci->laneAreaDetector(*area);  // Acquire connection to corresponding laneAreaDetector
        vehicleIDs.merge(LAD.getLastStepVehicleIDs());  // Append its vehicle IDs into the list
    }
    return vehicleIDs;
}

void RampMeterController::onWSM(BaseFrame1609_4* wsm) {

}

void RampMeterController::onWSA(DemoServiceAdvertisment* wsa) {

}


