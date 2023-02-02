#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "TrafficManagementCenter/TMC.h"

#define PARKNRIDE_MSG 103
#define CARAPP_VERBOSE 0

// List of possible exits to take to reach park and ride
static const std::vector<std::string> exitList = { // Exit no:
        "exit_to_I605",             // 0
        "exit_to_LAKEWOOD",         // 1
        "exit_to_PARAMOUNT_BLVD",   // 2
        "exit_to_SLAUSON_AVE",      // 3
        "exit_to_GARFIELD_AVE",     // 4
        "exit_to_WASHINGTON_BLVD",  // 5
        "exit_to_ATLANTIC_BLVD",    // 6
        "exit_to_ATLANTIC_TRIGGS",  // 7
        "exit_to_EASTERN_AVE",      // 8
        "exit_to_DITMAN_AVE",       // 9
        "exit_to_CALZONA_ST",       // A
        "destination_edge"          // B
};

// Travel time to park and ride in seconds (estimated from Google Maps)
static const std::vector<int> travelTimeList = {
        300,                        // exit_to_I605
        30,                         // exit_to_LAKEWOOD
        180,                        // exit_to_PARAMOUNT_BLVD
        300,                        // exit_to_SLAUSON_AVE
        360,                        // exit_to_GARFIELD_AVE
        540,                        // exit_to_WASHINGTON_BLVD
        720,                        // exit_to_ATLANTIC_BLVD
        840,                        // exit_to_ATLANTIC_TRIGGS
        900,                        // exit_to_EASTERN_AVE
        1080,                       // exit_to_DITMAN_AVE
        1200                        // exit_to_CALZONA_ST
};

enum {
    exit_to_I605,
    exit_to_LAKEWOOD,
    exit_to_PARAMOUNT_BLVD,
    exit_to_SLAUSON_AVE,
    exit_to_GARFIELD_AVE,
    exit_to_WASHINGTON_BLVD,
    exit_to_ATLANTIC_BLVD,
    exit_to_ATLANTIC_TRIGGS,
    exit_to_EASTERN_AVE,
    exit_to_DITMAN_AVE,
    exit_to_CALZONA_ST,
    UNASSIGNED
};

const std::string getExit(int exitNo) {
    if(exitNo >= 0 && exitNo < 11)
        return exitList[exitNo];
    return "destination_edge";
}

int getTravelTime(int exitNo) {
    if(exitNo >= 0 && exitNo < 11)
        return travelTimeList[exitNo];
    return 0;
}

namespace veins {

    class VEINS_API CarApp : public DemoBaseApplLayer {
    public:
        void initialize(int stage) override;
        void finish() override;
        /*
         * Command to tell vehicle to redirect to the park and ride structure using the next exit
         * Each road is labeled as "ne<exitNo>_<road_name>", and the exitNo dictates which exit
         * the vehicle will take. exitNo may be (0,1,2...9,A,B).
         */
        void redirect(void);

    protected:
        TMC *TMC_connection;
        rewards_t payloadReward = {0,SimTime::ZERO,0};
        // Important vehicle metadata
        simtime_t spawnTime;  // When vehicle was placed onto road
        uint8_t exitNo = UNASSIGNED;  // If redirected to park and ride,
        bool mainline_veh = false;
        std::string currentRoad;
        double CO2Emissions = 0.0;
        std::string vType;
        bool consideredRerouting = false;

        void onWSM(BaseFrame1609_4* wsm) override;
//        void onWSA(DemoServiceAdvertisment* wsa) override;
        void handlePositionUpdate(cObject* obj) override;
    };
}
