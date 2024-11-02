add_library(TestUtilitiesLib
    SerialInterfaceStub.cpp RecordHandler.cpp SensorRecord.cpp)
target_link_libraries(TestUtilitiesLib  UtilitiesLib LocalizationLib SensorLib PathPlanningLib)
