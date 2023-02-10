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

#ifndef CARAPPLICATION_BUSAPP_H_
#define CARAPPLICATION_BUSAPP_H_

#include <CarApplication/BasicCarApp.h>
#include "TrafficManagementCenter/TMC.h"

#define BUSAPP_VERBOSE 0

namespace veins {

class BusApp: public BasicCarApp {
public:
    BusApp();
    virtual ~BusApp();

    void initialize(int stage) override;
    void finish(void) override;
protected:
    TMC *TMC_connection;
    rewards_t payloadReward = {0,SimTime::ZERO,0};
    simtime_t spawnTime;
    double CO2Emissions = 0.0;
    /** @brief this function is called every time the vehicle receives a position update signal */
    void handlePositionUpdate(cObject* obj) override;
};

} /* namespace veins */

#endif /* CARAPPLICATION_BUSAPP_H_ */
