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
protected:
    void initialize(int stage) override;
    void handleSelfMsg(cMessage* msg) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    /* Which traffic light to control */
//    std::string trafficLightID;
};

}
#endif /* RAMPMETERCONTROL_RAMPMETERCONTROLLER_H_ */
