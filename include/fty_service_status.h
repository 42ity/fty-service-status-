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
#include <functional>
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
        Unknown        = 0, 
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
        Unknown                  = 0, 
        Ok                      = 5, 
        Warning                 = 10,
        MinorFailure            = 15,
        MajorFailure            = 20,
        CriticalFailure         = 25,
        NonRecoverableFailure   = 30
    };



    /// This class (Interface) is use to provide service status 
    ///
    /// The plugin can build such provider
    class ServiceStatusProvider
    {
        public:
        virtual ~ServiceStatusProvider(){};

        /// Get the service name
        ///@return  service name
        virtual const char * getServiceName() const noexcept  = 0;

        /// Set the Operating Status
        ///@param os [in] Operating Status to set
        ///@return 0 in success, negative number in case of error (see plugin documentation)
        virtual int set(OperatingStatus os) noexcept = 0;

        /// Set the Health State
        ///@param hs [in] Health state to set
        ///@return 0 in success, negative number in case of error (see plugin documentation)
        virtual int set(HealthState hs) noexcept = 0;
    };

    using ServiceStatusProviderPtr = std::shared_ptr<ServiceStatusProvider>;

    /// This class is a wrapper on a plugin which should implement service status funtions
    ///
    /// This class is in charge of the life cycle of the plugin accross the service
    /// Plugin wrapper must exist during the life of the object generated by the plugin.
    class ServiceStatusPluginWrapper
    {
        using FctNewSPP = int(*)(ServiceStatusProvider**, const char *);
        using FctDeleteSPP = void(*)(ServiceStatusProvider *);
        using FctGetString = const char * (*)();
        
        private:
        std::shared_ptr<void> m_spPlugin;

        FctGetString m_fctGetName;
        FctGetString m_fctGetLastError;
        FctNewSPP m_fctNewSPP;
        FctDeleteSPP m_fctDeleteSPP;

        public:
        /// Get the plugin name
        ///@return plugin name
        std::string getPluginName() const noexcept { return std::string(m_fctGetName()); }

        /// Create a ServiceStatusProvider
        ///@param serviceName [in] name of the service for which we do the notification
        ///@return sharedptr of ServiceStatusProvider with the correct deleter
        ServiceStatusProviderPtr newServiceStatusProviderPtr(const std::string & serviceName) {

            ServiceStatusProvider * ptr = nullptr;
            int error = m_fctNewSPP(&ptr, serviceName.c_str());
            if(error != 0) {
                throw std::runtime_error("Impossible to create the ServiceStatusProvider. Error "
                                            +std::to_string(error) +": " +m_fctGetLastError());
            }

            return ServiceStatusProviderPtr(ptr, m_fctDeleteSPP);
        }

        /// Create a ServiceStatusPluginWrapper
        /// @param pluginPath [in] path to the plugin
        ServiceStatusPluginWrapper(const std::string & pluginPath) {
            //try to load the plugin
            m_spPlugin = std::shared_ptr<void>(dlopen(pluginPath.c_str(), RTLD_NOW), [] (void * ptr) { if(ptr) dlclose(ptr);});
            if (!m_spPlugin) {
                throw std::runtime_error("Cannot load plugin: " + std::string(dlerror()));
            }

            //try to load the 3 functions
            // reset errors
            const char *dlsymError = dlerror();

            m_fctGetName = reinterpret_cast<FctGetString>(dlsym(m_spPlugin.get(), "getPluginName"));
            dlsymError = dlerror();
            if(dlsymError) {
                throw std::runtime_error("Cannot load function getPluginName: " + std::string(dlsymError));
            }

            m_fctNewSPP = reinterpret_cast<FctNewSPP>(dlsym(m_spPlugin.get(), "createServiceStatusProvider"));
            dlsymError = dlerror();
            if(dlsymError) {
                throw std::runtime_error("Cannot load function createServiceStatusProvider: " + std::string(dlsymError));
            }

            m_fctDeleteSPP = reinterpret_cast<FctDeleteSPP>(dlsym(m_spPlugin.get(), "deleteServiceStatusProvider"));
            dlsymError = dlerror();
            if(dlsymError) {
                throw std::runtime_error("Cannot load function deleteServiceStatusProvider: " + std::string(dlsymError));
            }

            m_fctGetLastError = reinterpret_cast<FctGetString>(dlsym(m_spPlugin.get(), "getPluginLastError"));
            dlsymError = dlerror();
            if(dlsymError) {
                throw std::runtime_error("Cannot load function getPluginLastError: " + std::string(dlsymError));
            }
        }
    };

    /// This class is all an easy use of collection of ServiceStatusPluginWrapper
    class ServiceStatusPluginWrapperCollection
    {
        private:
        std::string m_serviceName;
        std::map<std::string, ServiceStatusPluginWrapper> m_serviceStatusPluginWrappers;
        std::map<std::string, ServiceStatusProviderPtr> m_serviceStatusProviders;

        public:
        /// Create a ServiceStatusPluginWrapperCollection
        ServiceStatusPluginWrapperCollection(const std::string & serviceName) : m_serviceName(serviceName){}

        ~ServiceStatusPluginWrapperCollection(){
            //remove the objects first
            m_serviceStatusProviders.clear();

            //remove the plugins after
            m_serviceStatusPluginWrappers.clear();
        }

        /// Get the service name
        ///@return  service name
        const std::string & getServiceName() const noexcept {return m_serviceName;}

        /// Set the Health State for all the collection
        ///@param hs [in] Health state to set
        void setForAll(HealthState hs) noexcept { 
            for(auto & item : m_serviceStatusProviders)
            {
                item.second->set(hs);
            }
        }

        /// Set the Operating Status for all the collection
        ///@param os [in] Operating Status to set
        void setForAll(OperatingStatus os) noexcept { 
            for(auto & item : m_serviceStatusProviders)
            {
                item.second->set(os);
            }
        }

        /// Add a ServiceStatusProvider to the collection using the path to the plugin
        /// @param pluginPath [in] Path of the plugin
        void add(const std::string & pluginPath) {
            ServiceStatusPluginWrapper newPlugin(pluginPath);
            
            if(m_serviceStatusProviders.count(newPlugin.getPluginName()) > 0) {
                throw std::runtime_error("Plugin <"+newPlugin.getPluginName()+ "> already exist in the collection.");
            }

            m_serviceStatusProviders.emplace(newPlugin.getPluginName(), newPlugin.newServiceStatusProviderPtr(m_serviceName));
            m_serviceStatusPluginWrappers.emplace(newPlugin.getPluginName(), newPlugin);
        }

        /// Add all ServiceStatusProvider from a folder to the collection
        ///@param folderPath [in] Path to the folder
        ///@param regex [in] 
        ///@return number of added ServiceStatusProvider
        int addAll(const std::string & folderPath, const std::regex & regex = std::regex(".*")) {
            
            int added = 0;
            for(auto & path : listPathOfFolderElements(folderPath, regex)) {
                try {
                    add(path);
                    added++;
                }
                catch(...){}
            }

            return added;
        }

        /// Remove a ServiceStatusProvider to the collection using the name to the plugin
        /// @param pluginName [in] Path of the plugin to remove
        void remove( const std::string & pluginName ) noexcept { 
            m_serviceStatusProviders.erase(pluginName);
            m_serviceStatusPluginWrappers.erase(pluginName);
        }

        /// Get the ServiceStatusPluginWrapper from the collection.
        /// The ServiceStatusPluginWrapper are inside a map with there name as a key
        ///@return map of <name, ServiceStatusProvider>
        const std::map<std::string, ServiceStatusPluginWrapper> & getPluginCollection() const noexcept { return m_serviceStatusPluginWrappers; }

        /// Helper which list the content of a folder and return their full path if they match to the regex
        /// For example use "*.so" to get all the <file>.so path
        ///@param folderPath [in] Path to the folder
        ///@param regex [in] 
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