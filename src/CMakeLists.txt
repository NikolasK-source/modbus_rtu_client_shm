# ---------------------------------------- source files (*.cpp, *.cc, ...) ---------------------------------------------
# ======================================================================================================================

target_sources(${Target} PRIVATE main.cpp)
target_sources(${Target} PRIVATE modbus_shm.cpp)
target_sources(${Target} PRIVATE Modbus_RTU_Client.cpp)
target_sources(${Target} PRIVATE license.cpp)
target_sources(${Target} PRIVATE Print_Time.cpp)


# ---------------------------------------- header files (*.jpp, *.h, ...) ----------------------------------------------
# ======================================================================================================================
target_sources(${Target} PRIVATE modbus_shm.hpp)
target_sources(${Target} PRIVATE Modbus_RTU_Client.hpp)
target_sources(${Target} PRIVATE license.hpp)
target_sources(${Target} PRIVATE Print_Time.hpp)


# ---------------------------------------- subdirectories --------------------------------------------------------------
# ======================================================================================================================

add_subdirectory(generated)
