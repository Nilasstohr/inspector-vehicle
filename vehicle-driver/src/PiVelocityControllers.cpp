/*
 * PiVelocityControllers.cpp
 *
 *  Created on: Jun 5, 2023
 *      Author: robot1
 */

#include "PiVelocityControllers.h"

PiVelocityControllers::PiVelocityControllers(
		TransposedIIRFilter* piControllerFilterLeft,
		TransposedIIRFilter* piControllerFilterRight) {
	this->piControllerFilterLeft = piControllerFilterLeft;
	this->piControllerFilterRight= piControllerFilterRight;

}

TransposedIIRFilter* PiVelocityControllers::left() {
	return piControllerFilterLeft;
}

TransposedIIRFilter* PiVelocityControllers::right() {
	return piControllerFilterRight;
}

PiVelocityControllers::~PiVelocityControllers() {
	// TODO Auto-generated destructor stub
}

