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
#include "example.h"

#include <fstream>
#include <string>


//internal variables and functions
static std::string gPluginLastError = "";
static int writeToFile(const std::string & fullPath, uint8_t value);

//public interfaces
const char * getPluginName() {
    return "Example plugin";
}

const char * getPluginLastError(){
    return gPluginLastError.c_str();
}

int createServiceStatusProvider(fty::ServiceStatusProvider** spp, const char * serviceName) {
    try {
        *spp = dynamic_cast<fty::ServiceStatusProvider*>(new example::ServiceStatusExample(serviceName));
    }
    catch(const std::exception& e) {
        gPluginLastError = e.what();
        return -1;
    }

    gPluginLastError = "";
    return 0;
}

void deleteServiceStatusProvider(fty::ServiceStatusProvider* spp) {
    example::ServiceStatusExample * ptr = dynamic_cast<example::ServiceStatusExample*>(spp);
    delete ptr;
}


namespace example
{

    ServiceStatusExample::ServiceStatusExample(const char * serviceName)
        : m_serviceName(serviceName)
        {}

    const char * ServiceStatusExample::getServiceName() const noexcept {
        return m_serviceName.c_str();
    }

    /// Set the Operating Status
    ///@param os [in] Operating Status to set
    ///@return 0 in success, -1  in case of error and message is stored in getPluginLastError
    int ServiceStatusExample::set(fty::OperatingStatus os) noexcept {
        return writeToFile( m_serviceName + ".operating", static_cast<uint8_t>(os));
    }

    /// Set the Health State
    ///@param hs [in] Health state to set
    ///@return 0 in success, -1  in case of error and message is stored in getPluginLastError
    int ServiceStatusExample::set(fty::HealthState hs) noexcept {
        return writeToFile( m_serviceName + ".health", static_cast<uint8_t>(hs));
    }

} //namespace example

int setHealthState(const char * serviceName, uint8_t healthState)
{
    return writeToFile( std::string(serviceName) + ".health", healthState);
}

int setOperatingStatus(const char * serviceName, uint8_t operatingStatus) {
    return writeToFile( std::string(serviceName) + ".operating", operatingStatus);
}

static int writeToFile(const std::string & fullPath, uint8_t value) {
    try {
        std::ofstream outfile(fullPath, std::ofstream::trunc);
        outfile << std::to_string(value) << std::endl;
        outfile.close();
    }
    catch(std::exception & e) {
        gPluginLastError = e.what();
        return -1;
    }

    gPluginLastError = "";
    return 0;
}