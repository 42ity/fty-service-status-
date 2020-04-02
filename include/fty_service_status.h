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
#include <memory>
#include <map>
#include <list>
#include <regex>
#include <dirent.h> 

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

    /// This class is a wrapper on a plugin which should implement service status funtions
    ///
    /// This class is in charge of the life cycle of the plugin accross the service
    class ServiceStatusProvider
    {
        using FctWrapper = int(*)(const char *, uint8_t);

        private:
        std::string m_serviceName;
        std::string m_pluginPath;
        std::shared_ptr<void> m_spPlugin;
        FctWrapper m_fctSetOperatingStatus = nullptr;
        FctWrapper m_fctSetHealthState = nullptr;

        public:
        /// Set the Operating Status
        ///@param os [in] Operating Status to set
        ///@return 0 in success, -1 if function is not implemented, other number in case of error (see plugin documentation)
        int set(OperatingStatus os) noexcept {
            if(!m_fctSetOperatingStatus) return -1;
            
            return m_fctSetOperatingStatus( m_serviceName.c_str(), static_cast<uint8_t>(os));
        }

        /// Set the Health State
        ///@param hs [in] Health state to set
        ///@return 0 in success, -1 if function is not implemented, other number in case of error (see plugin documentation)
        int set(HealthState hs) noexcept { 
            if(!m_fctSetHealthState) return -1;
            return m_fctSetHealthState( m_serviceName.c_str(), static_cast<uint8_t>(hs));
        }

        /// Tell if the Set Operating Status is available
        ///@return true if the plugin offer the function
        bool isSetHealthStateAvailable() const noexcept { return (m_fctSetHealthState != nullptr); }

        /// Tell if the Set Health State is available in the 
        ///@return true if the plugin offer the function
        bool isSetOperatingStatusAvailable() const noexcept { return (m_fctSetOperatingStatus != nullptr); }

        /// Get the plugin path
        ///@return plugin path
        const std::string & getPluginPath() const noexcept { return m_pluginPath; }

        /// Get the service name
        ///@return  service name
        const std::string & getServiceName() const noexcept { return m_serviceName; }

        /// Create a ServiceStatusProvider
        /// @param serviceName [in] name of the service for which we do the notification
        /// @param pluginPath [in] path to the plugin
        ServiceStatusProvider(const std::string & serviceName, const std::string & pluginPath)
         : m_serviceName(serviceName){
            //try to load the plugin
            m_spPlugin = std::shared_ptr<void>(dlopen(pluginPath.c_str(), RTLD_LAZY), [] (void * ptr) { if(ptr) dlclose(ptr);});
            if (!m_spPlugin) {
                throw std::runtime_error("Cannot load plugin: " + std::string(dlerror()));
            }

            char * fullPath = realpath(pluginPath.c_str(), NULL);
            m_pluginPath = std::string(fullPath);
            free(fullPath);

            //try to load the 2 functions
            // reset errors
            const char *dlsymError = dlerror();
            m_fctSetHealthState = reinterpret_cast<FctWrapper>(dlsym(m_spPlugin.get(), "setHealthState"));
            dlsymError = dlerror();
            if(dlsymError) {
                m_fctSetHealthState = nullptr;
            }

            m_fctSetOperatingStatus = reinterpret_cast<FctWrapper>(dlsym(m_spPlugin.get(), "setOperatingStatus"));
            dlsymError = dlerror();
            if(dlsymError) {
                m_fctSetOperatingStatus = nullptr;
            }
        }
    };

    /// This class is all an easy use of collection of ServiceStatusProvider to set all status at once
    class ServiceStatusProviderCollection
    {
        private:
        std::string m_serviceName;
        std::map<std::string, ServiceStatusProvider> m_serviceStatusProviders;

        public:
        /// Create a ServiceStatusProviderCollection
        /// @param serviceName [in] name of the service for which we do the notification
        ServiceStatusProviderCollection(const std::string & serviceName): m_serviceName(serviceName) {};

        /// Get the service name
        ///@return  service name
        const std::string & getServiceName() const noexcept { return m_serviceName; }
        
        /// Set the Operating Status for all the collection
        ///@param os [in] Operating Status to set
        void setForAll(OperatingStatus os) noexcept {
            for(auto & item : m_serviceStatusProviders)
            {
                item.second.set(os);
            }
        }

        /// Set the Health State for all the collection
        ///@param hs [in] Health state to set
        void setForAll(HealthState hs) noexcept { 
            for(auto & item : m_serviceStatusProviders)
            {
                item.second.set(hs);
            }
        }

        /// Add a ServiceStatusProvider to the collection. The ServiceStatusProvider must be for the same service
        /// @param ssp [in] ServiceStatusProvider
        void add(const ServiceStatusProvider & ssp) { 
            if(m_serviceName != ssp.getServiceName()) {
                throw std::runtime_error( "Impossible to add a Service Status Provider for "+ssp.getServiceName()+" into a collection for "+m_serviceName);
            }

           m_serviceStatusProviders.emplace(ssp.getPluginPath(), ssp);
        }

        /// Add a ServiceStatusProvider to the collection using the path to the plugin
        /// @param pluginPath [in] Path of the plugin
        void add(const std::string & pluginPath) {
            ServiceStatusProvider newProvider(m_serviceName, pluginPath);
            m_serviceStatusProviders.emplace(newProvider.getPluginPath(), newProvider);
        }

        /// Get the ServiceStatusProvider from the collection.
        /// The ServiceStatusProviders are inside a map with there path as a key
        ///@return map of <path, ServiceStatusProvider>
        const std::map<std::string, ServiceStatusProvider> & getCollection() const noexcept { return m_serviceStatusProviders; }

        /// Remove a ServiceStatusProvider to the collection using the path to the plugin
        /// @param pluginPath [in] Path of the plugin to remove
        void remove( const std::string & pluginPath ) noexcept { m_serviceStatusProviders.erase(pluginPath); }

        /// Helper which list the content of a folder and return their full path if they match to the regex
        /// For example use "*.so" to get all the <file>.so path
        ///@param folderPath [in] Path to the folder
        ///@return List of paths
        static std::list<std::string> listPathOfFolderElements(const std::string & folderPath, const std::regex & regex = std::regex(".*")) {
            std::list<std::string> listPathElements;
            DIR *d;
            struct dirent *dir;
            d = opendir(folderPath.c_str());

            if (d) {
                while ((dir = readdir(d)) != NULL) {
                    if(std::regex_match(dir->d_name, regex)){
                        std::string path(folderPath +"/"+std::string(dir->d_name));
                        char * fullPath = realpath(path.c_str(), NULL);
                        listPathElements.push_back(fullPath);
                        free(fullPath);
                    }
                }
                closedir(d);
            }

            return listPathElements;
        }

    };

} //namespace fty