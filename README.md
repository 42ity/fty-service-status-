# fty-service-status
Helpers to provide status of service accross 42ity and more.

This library allow the service to use a plugin C++ architecture to "notify" of their status.
The list of operating status and health states available is discribe bellow.

This is a library header-only.
This project contain an example of a plugin implementation and unit tests

## How to build
```bash
mkdir build
cd build
cmake ..
make
make test # to run self-test
make memcheck # to run self-test with valgrind
make doc # to create doxygen documentation at the root of the project
```

## How to use the plugin and the objects provided by the plugin
### How to use one plugin
```cpp
#include <fty_service_status.h>
...
//load the plugin
fty::ServiceStatusPluginWrapper myPlugin("pathToMyPlugin");

//create a object to provide the status (The object is stored under a shared_ptr with a special deleter)
fty::ServiceStatusProviderPtr statusProvider = plugin.newServiceStatusProviderPtr("my-service");
...

statusProvider.set(fty::OperatingStatus::InService);
statusProvider.set(fty::HealthState::Ok);
...
```
Be carreful, the plugin (ServiceStatusPluginWrapper) must be loaded while ServiceStatusProvider objects are still in use.

### How to use the collection of plugins
```cpp
#include <fty_service_status.h>
...
//Create the collection
fty::ServiceStatusPluginWrapperCollection statusProviders("my-service");

//load all the plugin with a name finishing by "status.so"
int loaded = statusProviders.addAll("pathToMyPluginDirectory", std::regex(".*status.so"));
std::cout << "You loaded " << loaded << " service status provider plugins" << std::endl;
...

statusProviders.setForAll(fty::OperatingStatus::InService);
statusProviders.setForAll(fty::HealthState::Ok);
...
```

## List of available status
### Operating status
| Name  | Value | Comments  |
|-------|-------|-----------|
| Unknown | 0 | Indicates the implementation is in general capable of returning this property, but is unable to do so at this time.|
| None | 1 | Indicates that the implementation (provider) is capable of returning a value for this property, but not ever for this particular piece of hardware/software or the property is intentionally not used because it adds no meaningful information (as in the case of a property that is intended to add additional info to another property).|
| Servicing | 2 | Describes an element being configured, maintained, cleaned, or otherwise administered.|
| Starting | 3 | Describes an element being initialized.|
| Stopping | 4 | Describes an element being brought to an orderly stop.|
| Stopped | 5 | Are similar, although the former implies a clean and orderly stop, while the latter implies an abrupt stop where the state and configuration of the element might need to be updated.|
| Aborted | 6 | |
| Dormant | 7 | Indicates that the element is inactive or quiesced.|
| Completed | 8 | Indicates that the element has completed its operation. This value should be combined with either OK, Error, or Degraded in the PrimaryStatus so that a client can tell if the complete operation Completed with OK (passed), Completed with Error (failed), or Completed with Degraded (the operation finished, but it did not complete OK or did not report an error).|
| Migrating | 9 | Element is being moved between host elements.|
| Immigrating | 10 | Element is being moved to new host element.|
| Emigrating | 11 | Element is being moved away from host element.|
| Snapshotting | 12 ||
| Shutting Down | 13 | Describes an element being brought to an abrupt stop|
| In Test | 14 | Element is performing test functions.|
| Transitioning | 15 | Describes an element that is between states, that is, it is not fully available in either its previous state or its next state. This value should be used if other values indicating a transition to a specific state are not applicable.|
| In Service | 16 | Describes an element that is in service and operational.|

### Health states
| Name  | Value | Comments  |
|-------|-------|-----------|
| Unknown | 0 | The implementation cannot report on HealthState at this time. |
| OK | 5 | The element is fully functional and is operating within normal operational parameters and without error.|
| Warning | 10 | The element is in working order and all functionality is provided. However, the element is not working to the best of its abilities. For example, the element might not be operating at optimal performance or it might be reporting recoverable errors.|
| Minor Failure | 15 | All functionality is available but some might be degraded.|
| Major Failure | 20 | The element is failing. It is possible that some or all of the functionality of this component is degraded or not working.|
| Critical Failure | 25 | The element is non-functional and recovery might not be possible.|
| Non-recoverable Error | 30 | The element has completely failed, and recovery is not possible. All functionality provided by this element has been lost.|

## How to create a plugin

A plugin must implement the following functions:

```cpp
///Return the plugin name
const char * getPluginName();

///Return the last error message from the plugin
const char * getPluginLastError();

///Allocate a ServiceStatusProvider in the pointer give in parameter
int createServiceStatusProvider(fty::ServiceStatusProvider** spp, const char * serviceName);

///Free the ServiceStatusProvider in parameter
void deleteServiceStatusProvider(fty::ServiceStatusProvider* spp);
```
If you wonder why we have a deleter function. Remember that for every memory allocation done on the plugin, the memory must be free in the plugin to avaoid bad surprise. Same with the types we use: ServiceStatusProvider do not have functions using c++ standard library objects. It's to avoid issue created when compilers of application and lib do not use the same implementation of the c++ standard library. In your plugin implementation, you can use the standare library.
