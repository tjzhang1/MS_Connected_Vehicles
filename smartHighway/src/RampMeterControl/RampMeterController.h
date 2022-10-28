/*
 * RampMeterController.h
 *
 *  Created on: Apr 7, 2022
 *      Author: tjzhang
 */

#ifndef RAMPMETERCONTROL_RAMPMETERCONTROLLER_H_
#define RAMPMETERCONTROL_RAMPMETERCONTROLLER_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/world/traci/trafficLight/TraCITrafficLightInterface.h"

enum {
    RMC_ALINEA_MEASURE_MSG = 100,
    RMC_ALINEA_UPDATE_MSG = 101,
    RMC_SET_GREEN_MSG = 102,
};

#define RMC_VERBOSE 1

namespace veins {

class RampMeterController : public DemoBaseApplLayer {
public:
    RampMeterController(void);
    ~RampMeterController(void);
protected:
    void initialize(int stage) override;
    void handleSelfMsg(cMessage* msg) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void stringListFromParam(std::vector<std::string> &list, const char *parName);
    void handleMessage(cMessage* msg) override;
    /* Which traffic light to control */
    TraCITrafficLightInterface* tlInterface;
    int updatePeriodALINEA = 60; //period of time to use the next ALINEA ramp metering rate: [40, 300] seconds
    double onRampOccupancy; //accumulated occupancy measured during updatePeriodALINEA
    double hwyOccupancy;
    /*
     * From Caltrans Ramp Metering Design Manual:
    For a typical one vehicle per green operation, a
    ramp meter has practical lower and upper output
    limits of 240 and 900 vehicles per hour (VPH) per
    lane, respectively. Ramp metering signals set for
    flow rates outside this range tend to have high
    violation rates and cannot effectively control
    traffic.
    */
    // In SUMO, can't force 1 vehicle per green.
    // 240 VPH = 4 VPM => 15 seconds down time between greens
    // 900 VPH = 15 VPM => 4 seconds down time between greens
    double rMax = 900;
    double rMin = 240;
    double meterFlow;
    double meterRate; //time between green lights
    double KR = 70.0;
    double targetOccupancy = 20.0;

    std::vector<std::string> highwayInductorsList;
    std::vector<std::string> onRampInductorsList;
    std::string controlType;

    cMessage *measureMsg;
    cMessage *updateMsg;
    cMessage *changePhaseMsg;
};

}
#endif /* RAMPMETERCONTROL_RAMPMETERCONTROLLER_H_ */
