/*
 * RampMeterController.cc
 *
 *  Created on: Apr 7, 2022
 *      Author: tjzhang
 */

#include "RampMeterController.h"

using namespace veins;

Define_Module(veins::RampMeterController);

void RampMeterController::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    // Read which traffic light to control from omnetpp.ini file
    trafficLightID = par("rampMeterID");
    // obtain control of Traci commands for traffic light
    rampMeter = traci->trafficlight(trafficLightID);
    rampMeter.setProgram("my_program");
}

