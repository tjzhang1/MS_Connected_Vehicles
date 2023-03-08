#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "TrafficManagementCenter/TMC.h"

#define PARKNRIDE_MSG 103
#define CARAPP_VERBOSE 0
#define HOV_INTERVAL 60  //num seconds between each bus
#define COMPLIANCE 5.0  //out of 100.0, percentage chance for vehicle to reroute

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

    /*
     * It is assumed that this application will be assigned only to vehicles whose destinations are the mainline sink,
     * and that this vehicle is not an HOV or continuing vehicle.
     */
    class VEINS_API CarApp : public DemoBaseApplLayer {
    public:
        void initialize(int stage) override;
        void finish() override;
        /*
         * Command to tell vehicle to redirect to the park and ride structure using the next exit
         * Each road is labeled with a "ne<exitNo>" prefix, and the exitNo dictates which exit
         * the vehicle will take. exitNo may be (0,1,2...9,A,B).
         */
        void redirect(void);

    protected:
        TMC *TMC_connection;
        simtime_t spawnTime;
        uint8_t exitNo = UNASSIGNED;  // If redirected to park and ride, will be updated with value from enumerated exits
        double CO2Emissions = 0.0;
        bool consideredRerouting = false;

        void onWSM(BaseFrame1609_4* wsm) override;
        void handlePositionUpdate(cObject* obj) override;
    };
}
