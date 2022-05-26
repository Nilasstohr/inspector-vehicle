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


class CQuadJoyStickDriver {
	public:
		CQuadJoyStickDriver(
		    MovingAverageFilter * pMovingAverageFilter,
			uint16_t iResolution,
			uint8_t iChannel1,
			uint8_t iChannel2,
			uint8_t iVoltageMin,
			uint8_t iVoltageMax
		);

		void init();
		float GetThrotleVoltage();
		float GetThrotleVoltageFilter();

		uint16_t GetVoltageMax();
		uint16_t GetVoltageMin();


		virtual ~CQuadJoyStickDriver();

	private:

		typedef struct sJoyStick
		{
			float Channel;
			float Voltage;
		}JoyStick;

		JoyStick  *m_JS_1;
		uint16_t m_iResolution;
		float    m_fConv;
		uint16_t m_iVoltageMin;
		uint16_t m_iVoltageMax;

        MovingAverageFilter * m_pMovingAverageFilter;

};

#endif /* QUADJOYSTICKDRIVER_H_ */
