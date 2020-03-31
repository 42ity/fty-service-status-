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

#include <cstdint>
#include <string>
#include <stdexcept>

#include <dlfcn.h>

namespace fty
{
    enum class OperatingStatus : std::uint8_t
    { 
        Unkown        = 0, 
        None          = 1, 
        Servicing     = 2,
        Starting      = 3,
        Stopping      = 4,
        Stopped       = 5,
        Aborted       = 6,
        Dormant       = 7,
        Completed     = 8,
        Migrating     = 9,
        Immigrating   = 10,
        Emigrating    = 11,
        Snapshotting  = 12,
        ShuttingDown  = 13,
        InTest        = 14,
        Transitioning = 15,
        InService     = 16
    };

    enum class HealthState : std::uint8_t
    { 
        Unkown                  = 0, 
        Ok                      = 5, 
        Warning                 = 10,
        MinorFailure            = 15,
        MajorFailure            = 20,
        CriticalFailure         = 25,
        NonRecoverableFailure   = 30
    };

    /// This class is a wrapper on a dynamic library which should implement service status funtions
    ///
    /// This class is in charge of the life cycle of the dynamic library accross the service
    /// @param serviceName [in] name of the service for which we do the notification
    /// @param libraryPath [in] path to the dynamic library
    class ServiceStatusProvider
    {
        using FctWrapper = int(*)(const char *, uint8_t);

        private:
        std::string m_serviceName;
        void * m_pLib;
        FctWrapper m_fctSetOperatingStatus;
        FctWrapper m_fctSetHealthState;

        public:
        /// Set the Operating Status
        ///@param os [in] Operating Status to set
        ///@return 0 in success, other number in case of error (see library documentation)
        int set(OperatingStatus os) { return m_fctSetOperatingStatus( m_serviceName.c_str(), static_cast<uint8_t>(os)); }

        /// Set the Health status
        ///@param hs [in] Health status to set
        ///@return 0 in success, other number in case of error (see library documentation)
        int set(HealthState hs) { return m_fctSetHealthState( m_serviceName.c_str(), static_cast<uint8_t>(hs)); }

        ServiceStatusProvider(const std::string & serviceName, const std::string & libraryPath)
         : m_serviceName(serviceName) {
            
            //try to load the library
            m_pLib = dlopen(libraryPath.c_str(), RTLD_LAZY);
            if (!m_pLib) {
                throw std::runtime_error("Cannot load library: " + std::string(dlerror()));
            }

            //try to load the 2 functions
            // reset errors
            const char *dlsymError = dlerror();
            m_fctSetHealthState = reinterpret_cast<FctWrapper>(dlsym(m_pLib, "setHealthState"));
            dlsymError = dlerror();
            if(dlsymError) {
                dlclose(m_pLib);
                throw std::runtime_error("Cannot load function <setHealthState> from " + libraryPath + ": " + std::string(dlsymError));
            }

            m_fctSetOperatingStatus = reinterpret_cast<FctWrapper>(dlsym(m_pLib, "setOperatingStatus"));
            dlsymError = dlerror();
            if(dlsymError) {
                dlclose(m_pLib);
                throw std::runtime_error("Cannot load function <setOperatingStatus> from " + libraryPath + ": " + std::string(dlsymError));
            }
        }
        
        ~ServiceStatusProvider() { dlclose(m_pLib); }
    };

    /// This class is all an easy use of collection of ServiceStatusProvider
    class ServiceStatusCollection
    {

    };

} //namespace fty