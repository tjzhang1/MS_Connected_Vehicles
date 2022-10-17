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

#define RSU_VERBOSE 1
#define SAMPLING_PERIOD 8

namespace veins {

class RSUApp : public DemoBaseApplLayer {
public:
    RSUApp();
    ~RSUApp();
    void initialize(int stage) override;
    void finish() override;
    enum RSU_MSG_types {
        RSU_BROADCAST_MSG,    // Tells RSU to broadcast an advisory to cars
        RSU_SAMPLE_MSG,       // Tells RSU to collect information on nearby cars
    };
protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void sendToTMC(void);
    void stringListFromParam(std::vector<std::string> &list, const char *parName);
    std::list<std::string> sampleAreaDetectors(void);
    void handleSelfMsg(cMessage *msg) override;
private:
    std::vector<std::string> areaDetectorList;
    cMessage *samplingMsg;
};

}
#endif /* RSUAPP_RSUAPP_H_ */
