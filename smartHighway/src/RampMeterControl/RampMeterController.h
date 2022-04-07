/*
 * RampMeterController.h
 *
 *  Created on: Apr 7, 2022
 *      Author: tjzhang
 */

#ifndef RAMPMETERCONTROL_RAMPMETERCONTROLLER_H_
#define RAMPMETERCONTROL_RAMPMETERCONTROLLER_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace veins {

class RampMeterController : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
protected:
    /* Which traffic light to control */
    std::string trafficLightID;
    TraCICommandInterface::Trafficlight rampMeter;
};

}
#endif /* RAMPMETERCONTROL_RAMPMETERCONTROLLER_H_ */
