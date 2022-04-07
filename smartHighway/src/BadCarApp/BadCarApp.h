#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace veins {

    class VEINS_API BadCarApp : public DemoBaseApplLayer {
    public:
        void initialize(int stage) override;

    protected:
        simtime_t lastMsgAt;
        bool sentMessage;
        int currentSubscribedServiceId;

    protected:
        void onWSM(BaseFrame1609_4* wsm) override;
        void onWSA(DemoServiceAdvertisment* wsa) override;

        void handleSelfMsg(cMessage* msg) override;
        void handlePositionUpdate(cObject* obj) override;

        void populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type rcvId = LAddress::L2BROADCAST(), int serial = 0) override;
    };
}
