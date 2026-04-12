//
// Created by robot1 on 8/23/24.
//

#ifndef ROBOTVEHICLE_PACKAGE_SERIALINTERFACESTUB_H
#define ROBOTVEHICLE_PACKAGE_SERIALINTERFACESTUB_H

#include "Utilities/SerialInterface.h"

class SerialInterfaceStub: public SerialInterface{
public:
    explicit SerialInterfaceStub(const char *serialDevice);
    void sendRequest(const char *text) override;
    void sendRequest(std::string *text) override;
};


#endif //ROBOTVEHICLE_PACKAGE_SERIALINTERFACESTUB_H
