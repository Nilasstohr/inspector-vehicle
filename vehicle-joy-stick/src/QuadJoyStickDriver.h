/*
 * QuadJoyStickDriver.h
 *
 *  Created on: 31/05/2019
 *      Author: nist
 */

#ifndef QUADJOYSTICKDRIVER_H_
#define QUADJOYSTICKDRIVER_H_

#include "core_pins.h"
#include <stdlib.h>
#include "MovingAverageFilter.h"


class QuadJoyStickDriver {
	public:
		QuadJoyStickDriver(
			uint16_t iResolution,
			uint8_t iChannel,
			uint8_t iVoltageMin,
			uint8_t iVoltageMax
		);

		void init();
		float GetThrotleVoltage();
		float GetVerticalVoltageFilter();

		uint16_t GetVoltageMax();
		uint16_t GetVoltageMin();


		virtual ~QuadJoyStickDriver();

	private:

		typedef struct sJoyStick
		{
			float Channel;
			float Voltage;
		}JoyStick;

		JoyStick  *m_JS_1;
		JoyStick  *m_JS_2;
		MovingAverageFilter * m_pMovingAverageFilter;
		uint16_t m_iResolution;
		float    m_fConv;
		uint16_t m_iVoltageMin;
		uint16_t m_iVoltageMax;

        ;

};

#endif /* QUADJOYSTICKDRIVER_H_ */
