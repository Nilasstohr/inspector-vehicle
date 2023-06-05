/*
 * PiVelocityControllers.h
 *
 *  Created on: Jun 5, 2023
 *      Author: robot1
 */

#ifndef SRC_PIVELOCITYCONTROLLERS_H_
#define SRC_PIVELOCITYCONTROLLERS_H_

#include "TransposedIIRFilter.h"

class PiVelocityControllers {
public:
	PiVelocityControllers(
			TransposedIIRFilter* piControllerFilterLeft,
			TransposedIIRFilter* piControllerFilterRight);
	TransposedIIRFilter* left();
	TransposedIIRFilter* right();
	virtual ~PiVelocityControllers();
private:
	TransposedIIRFilter* piControllerFilterLeft;
	TransposedIIRFilter* piControllerFilterRight;
};

#endif /* SRC_PIVELOCITYCONTROLLERS_H_ */
