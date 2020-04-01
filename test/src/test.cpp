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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

TEST_CASE( "Link to valid plugin (example lib)", "[fty::ServiceStatusProvider] 1" ) {
    REQUIRE_NOTHROW(fty::ServiceStatusProvider("test-service", "../example/libfty-service-status-example.so"));
}

TEST_CASE( "Link to not existing plugin", "[fty::ServiceStatusProvider] 2" ) {
    REQUIRE_THROWS(fty::ServiceStatusProvider("test-service", "do-not-exist.so"));
}

TEST_CASE( "Test the output of example plugin", "[fty::ServiceStatusProvider] 3" ) {

    fty::ServiceStatusProvider statusProvider("test-service", "../example/libfty-service-status-example.so");
    
    SECTION( "set an operating status and read it back" ) {

        statusProvider.set(fty::OperatingStatus::InService);

        std::ifstream operating("test-service.operating");
        std::string sOperatingStatus;
        operating >> sOperatingStatus;

        uint8_t operatingStatus = std::stoi(sOperatingStatus);

        REQUIRE( static_cast<fty::OperatingStatus>(operatingStatus) == fty::OperatingStatus::InService );

    }
    
    SECTION( "set an operating status and read it back" ) {

        statusProvider.set(fty::HealthState::Ok);

        std::ifstream operating("test-service.health");
        std::string sOperatingStatus;
        operating >> sOperatingStatus;

        uint8_t operatingStatus = std::stoi(sOperatingStatus);

        REQUIRE( static_cast<fty::HealthState>(operatingStatus) == fty::HealthState::Ok );
    }
}