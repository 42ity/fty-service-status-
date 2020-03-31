/*  ========================================================================
    Copyright (C) 2020 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    ========================================================================
*/

//This test try to load the example and check the result file located next to the executable

//we should refactor it with catch2 when it will be available with debian 10.

#include <fty_service_status.h>

#include <unistd.h>
#include <iostream>
#include <fstream>

int main () {
    try
    {
        fty::ServiceStatusProvider statusProvider("test-service", "../example/libfty-service-status-example.so");

        //set a operating status and read it back
        {
            statusProvider.set(fty::OperatingStatus::InService);

            std::ifstream operating("test-service.operating");
            std::string sOperatingStatus;
            operating >> sOperatingStatus;

            uint8_t operatingStatus = std::stoi(sOperatingStatus);

            if( static_cast<fty::OperatingStatus>(operatingStatus) != fty::OperatingStatus::InService ) {
                return -1;
            }
        }
       
        //set an operating status and read it back
        {
            statusProvider.set(fty::HealthState::Ok);

            std::ifstream operating("test-service.health");
            std::string sOperatingStatus;
            operating >> sOperatingStatus;

            uint8_t operatingStatus = std::stoi(sOperatingStatus);

            if( static_cast<fty::HealthState>(operatingStatus) != fty::HealthState::Ok ) {
                return -1;
            }
        }
    }
    catch(std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    
    return 0;
} 