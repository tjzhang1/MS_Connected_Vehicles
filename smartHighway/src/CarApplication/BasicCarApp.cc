/*
 * BusApp.cpp
 *
 *  Created on: Feb 9, 2023
 *      Author: tjzhang
 */

#include <CarApplication/BasicCarApp.h>

using namespace veins;

BasicCarApp::BasicCarApp() {
    // TODO Auto-generated constructor stub
}

BasicCarApp::~BasicCarApp() {
    findHost()->unsubscribe(BaseMobility::mobilityStateChangedSignal, this);
}

void BasicCarApp::initialize(int stage) {
    BaseModule::initialize(stage);
    if(stage == 0) {
        // initialize pointers to other modules
        if (FindModule<TraCIMobility*>::findSubModule(getParentModule())) {
            mobility = TraCIMobilityAccess().get(getParentModule());
            traci = mobility->getCommandInterface();
            traciVehicle = mobility->getVehicleCommandInterface();
        }
        else {
            traci = nullptr;
            mobility = nullptr;
            traciVehicle = nullptr;
        }
        findHost()->subscribe(BaseMobility::mobilityStateChangedSignal, this);
    }
}

void BasicCarApp::finish() {
}

void BasicCarApp::receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details) {
    Enter_Method_Silent();
    if (signalID == BaseMobility::mobilityStateChangedSignal) {
        handlePositionUpdate(obj);
    }
}

void BasicCarApp::handlePositionUpdate(cObject *obj) {
}
