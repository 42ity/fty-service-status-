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

const std::string EXAMPLE_FOLDER = "../example/";
const std::string EXAMPLE_NAME = "libfty-service-status-example.so";
const std::string EXAMPLE_PATH = EXAMPLE_FOLDER + EXAMPLE_NAME;

TEST_CASE( "Link to valid plugin (example lib)", "[fty::ServiceStatusProvider]-valid" ) {
    REQUIRE_NOTHROW(fty::ServiceStatusProvider("test-service", EXAMPLE_PATH));
}

TEST_CASE( "Link to not existing plugin", "[fty::ServiceStatusProvider]-notValid" ) {
    REQUIRE_THROWS(fty::ServiceStatusProvider("test-service", "do-not-exist.so"));
}

TEST_CASE( "Test content plugin", "[fty::ServiceStatusProvider]-functions" ) {
    fty::ServiceStatusProvider p1("test-service", EXAMPLE_PATH);

    REQUIRE(p1.isSetHealthStateAvailable());
    REQUIRE(p1.isSetOperatingStatusAvailable());
    REQUIRE(p1.getServiceName() == "test-service");
}

TEST_CASE( "Copy of an existing plugin (memleak at copy)", "[fty::ServiceStatusProvider]-copy" ) {
    fty::ServiceStatusProvider p1("test-service", EXAMPLE_PATH);
    fty::ServiceStatusProvider p2 = p1;

    REQUIRE(p1.getPluginPath() == p2.getPluginPath());
}

TEST_CASE( "Test the output of example plugin", "[fty::ServiceStatusProvider]-set" ) {

    fty::ServiceStatusProvider statusProvider("test-service", EXAMPLE_PATH);
    
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

TEST_CASE( "Test collection add from path", "[fty::ServiceStatusProviderCollection]-addFromPath" ) {
    fty::ServiceStatusProviderCollection collection("test-service");

    REQUIRE(collection.getServiceName() == "test-service");
    REQUIRE(collection.getCollection().size() == 0);

    REQUIRE_NOTHROW(collection.add(EXAMPLE_PATH));
    REQUIRE(collection.getCollection().size() == 1);
}

TEST_CASE( "Test collection add wrong from path", "[fty::ServiceStatusProviderCollection]-addWromgFromPath" ) {
    fty::ServiceStatusProviderCollection collection("test-service");

    REQUIRE(collection.getServiceName() == "test-service");
    REQUIRE(collection.getCollection().size() == 0);

    REQUIRE_THROWS(collection.add("nothing.so"));
    REQUIRE(collection.getCollection().size() == 0);
}

TEST_CASE( "Test collection add from copy", "[fty::ServiceStatusProviderCollection]-addFromCopy" ) {
    fty::ServiceStatusProviderCollection collection("test-service");

    REQUIRE(collection.getServiceName() == "test-service");
    REQUIRE(collection.getCollection().size() == 0);

    fty::ServiceStatusProvider statusProvider1("test-service", EXAMPLE_PATH);
    REQUIRE_NOTHROW(collection.add(statusProvider1));
    REQUIRE(collection.getCollection().size() == 1);
}

TEST_CASE( "Test collection add wrong from copy", "[fty::ServiceStatusProviderCollection]-addWrongFromCopy" ) {
    fty::ServiceStatusProviderCollection collection("test-service");

    REQUIRE(collection.getServiceName() == "test-service");
    REQUIRE(collection.getCollection().size() == 0);

    fty::ServiceStatusProvider statusProvider2("test-service2", EXAMPLE_PATH);
    REQUIRE_THROWS(collection.add(statusProvider2));
    REQUIRE(collection.getCollection().size() == 0);
}

TEST_CASE( "Test collection remove", "[fty::ServiceStatusProviderCollection]-remove" ) {
    fty::ServiceStatusProviderCollection collection("test-service");

    REQUIRE(collection.getServiceName() == "test-service");
    REQUIRE(collection.getCollection().size() == 0);

    fty::ServiceStatusProvider statusProvider1("test-service", EXAMPLE_PATH);
    REQUIRE_NOTHROW(collection.add(statusProvider1));
    REQUIRE(collection.getCollection().size() == 1);

    REQUIRE_NOTHROW(collection.remove(statusProvider1.getPluginPath()));
    REQUIRE(collection.getCollection().size() == 0);
}

TEST_CASE( "Test collection set with empty collection", "[fty::ServiceStatusProviderCollection]-emptySet" ) {
    fty::ServiceStatusProviderCollection collection("test-service");

    REQUIRE(collection.getServiceName() == "test-service");
    REQUIRE(collection.getCollection().size() == 0);

    REQUIRE_NOTHROW(collection.setForAll(fty::OperatingStatus::Aborted));
    REQUIRE_NOTHROW(collection.setForAll(fty::HealthState::MinorFailure));
}

TEST_CASE( "Test collection set with example", "[fty::ServiceStatusProviderCollection]-exampleSet" ) {
    fty::ServiceStatusProviderCollection collection("test-service");

    REQUIRE_NOTHROW(collection.add(EXAMPLE_PATH));

        SECTION( "set an operating status and read it back" ) {

        collection.setForAll(fty::OperatingStatus::Emigrating);

        std::ifstream operating("test-service.operating");
        std::string sOperatingStatus;
        operating >> sOperatingStatus;

        uint8_t operatingStatus = std::stoi(sOperatingStatus);

        REQUIRE( static_cast<fty::OperatingStatus>(operatingStatus) == fty::OperatingStatus::Emigrating );

    }
    
    SECTION( "set an operating status and read it back" ) {

        collection.setForAll(fty::HealthState::NonRecoverableFailure);

        std::ifstream operating("test-service.health");
        std::string sOperatingStatus;
        operating >> sOperatingStatus;

        uint8_t operatingStatus = std::stoi(sOperatingStatus);

        REQUIRE( static_cast<fty::HealthState>(operatingStatus) == fty::HealthState::NonRecoverableFailure );
    }

}

TEST_CASE( "Test listPathOfFolderElements", "[fty::ServiceStatusProviderCollection]-listPathOfFolderElements" ) {
    std::list<std::string> list = fty::ServiceStatusProviderCollection::listPathOfFolderElements(EXAMPLE_FOLDER, std::regex(".*\\.so"));

    REQUIRE(list.size() == 1);
}

