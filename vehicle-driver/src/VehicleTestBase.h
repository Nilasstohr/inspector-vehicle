/*
 * VehicleTestBase.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_VEHICLETESTBASE_H_
#define SRC_VEHICLETESTBASE_H_

#include "VehicleTestToolBox.h"
#include "StringUtilities.h"

class VehicleTestBase: protected VehicleTestToolBox{
public:
	VehicleTestBase();
	virtual ~VehicleTestBase();
};

#endif /* SRC_VEHICLETESTBASE_H_ */
