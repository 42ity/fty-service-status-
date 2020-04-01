# fty-service-status
Helpers to provide status of service accross 42ity and more.

This library allow the service to use a plugin architecture to "notify" of their status.
The list of operating status and health states availlable is discribe bellow.

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
## List of availlable status
### Operating status

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