#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "TrafficManagementCenter/TMC.h"

#define PARKNRIDE_MSG 103
namespace veins {

    class VEINS_API CarApp : public DemoBaseApplLayer {
    public:
        void initialize(int stage) override;
        void finish() override;

    protected:
        simtime_t lastDroveAt;
        bool sentMessage;
        int currentSubscribedServiceId;
        TMC *TMC_connection;
        simtime_t spawnTime;
        rewards_t payloadReward = {0};

    protected:
        void onWSM(BaseFrame1609_4* wsm) override;
        void onWSA(DemoServiceAdvertisment* wsa) override;

        void handleSelfMsg(cMessage* msg) override;
        void handlePositionUpdate(cObject* obj) override;
    };
}
