/*
 * JoyStickController.h
 *
 *  Created on: Nov 12, 2023
 *      Author: robot1
 */

#ifndef SRC_JOYSTICKCONTROLLER_H_
#define SRC_JOYSTICKCONTROLLER_H_

#include "QuadJoyStickDriver.h"

class JoyStickController {
public:
	JoyStickController(uint16_t resolution,
			uint8_t analogVerPin,
			uint8_t analogHorPin,
			uint8_t voltageMin,
			uint8_t voltageMax);
	QuadJoyStickDriver * getVerticalControl();
	QuadJoyStickDriver * getHorisontalControl();

	virtual ~JoyStickController();
private:
	QuadJoyStickDriver * vertical;
	QuadJoyStickDriver * horisontal;
};

#endif /* SRC_JOYSTICKCONTROLLER_H_ */
