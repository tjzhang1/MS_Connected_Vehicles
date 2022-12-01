//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef RSUAPP_RSUAPP_H_
#define RSUAPP_RSUAPP_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "Messaging/RSU_Data_m.h"

#define RSU_VERBOSE 1
#define UPDATE_TMC_PERIOD 50  // How often to send data to TMC
#define ACCUM_DATA_PERIOD 5  // How often to accumulate network data, must be < UPDATE_TMC_PERIOD
enum RSU_MSG_types {
    RSU_BROADCAST_MSG,    // Tells RSU to broadcast an advisory to cars
    RSU_SAMPLE_MSG,       // Tells RSU to accumulate information from area detectors
    RSU_UPDATE_TMC_MSG,   // Tells RSU to send collected data to TMC
};

namespace veins {

class RSUApp : public DemoBaseApplLayer {
public:
    RSUApp();
    ~RSUApp();
    void initialize(int stage) override;
    void finish() override;
protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void sendToTMC(std::list<std::string> &vehicleIDs);
    void stringListFromParam(std::vector<std::string> &list, const char *parName);
    std::list<std::string> getVehicleIDs(void);
    void handleMessage(cMessage *msg) override;
    void populateData(RSU_Data *data, std::list<std::string> &vehicleIDs);
    // Reset the measured network statistics
    void resetStatistics(void);
private:
    std::vector<std::string> areaDetectorList;
    cMessage *updateTMC_msg, *sampleMsg;
    TraCICommandInterface *traci;
    // Measured statistics
    double accum_occupancy;
    double accum_speed;
    int accum_halting_vehicles;
    int samplesCount;
};

}
#endif /* RSUAPP_RSUAPP_H_ */
