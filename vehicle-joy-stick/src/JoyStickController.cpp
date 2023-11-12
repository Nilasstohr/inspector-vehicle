/*
 * JoyStickController.cpp
 *
 *  Created on: Nov 12, 2023
 *      Author: robot1
 */

#include "JoyStickController.h"

JoyStickController::JoyStickController(
		uint16_t resolution,
		uint8_t analogVerPin,
		uint8_t analogHorPin,
		uint8_t voltageMin,
		uint8_t voltageMax) {
	// TODO Auto-generated constructor stub
	vertical = new QuadJoyStickDriver(
			resolution,
			analogVerPin,
			voltageMin,
			voltageMax
	);
	horisontal = new QuadJoyStickDriver(
			resolution,
			analogHorPin,
			voltageMin,
			voltageMax
	);
}

QuadJoyStickDriver* JoyStickController::getVerticalControl() {
	return vertical;
}

JoyStickController::~JoyStickController() {
	// TODO Auto-generated destructor stub
}

QuadJoyStickDriver* JoyStickController::getHorisontalControl() {
	return horisontal;
}
