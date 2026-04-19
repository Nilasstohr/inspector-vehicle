add_library(TestUtilitiesLib
    SerialInterfaceStub.cpp)
target_link_libraries(TestUtilitiesLib  UtilitiesLib  HostLib )
