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
#pragma once

#include <fty_service_status.h>

//public interfaces
extern "C"
{
    const char * getPluginName();
    const char * getPluginLastError();
    int createServiceStatusProvider(fty::ServiceStatusProvider** spp, const char * serviceName);
    void deleteServiceStatusProvider(fty::ServiceStatusProvider* spp);
}

namespace example
{
    //implementation of the example ServiceStatus
    class ServiceStatusExample : public fty::ServiceStatusProvider
    {
        private:
        std::string m_serviceName;

        public:
        ServiceStatusExample( const char * serviceName);

        /// Get the service name
        ///@return  service name
        const char * getServiceName() const noexcept override;

        /// Set the Operating Status
        ///@param os [in] Operating Status to set
        ///@return 0 in success, -1  in case of error and message is stored in getPluginLastError
        int set(fty::OperatingStatus os) noexcept override;

        /// Set the Health State
        ///@param hs [in] Health state to set
        ///@return 0 in success, -1  in case of error and message is stored in getPluginLastError
        int set(fty::HealthState hs) noexcept override;
        
    };

} //namespace example

