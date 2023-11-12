/*
 * QuadJoyStickDriver.cpp
 *
 *  Created on: 31/05/2019
 *      Author: nist
 */

#include "QuadJoyStickDriver.h"

QuadJoyStickDriver::QuadJoyStickDriver(
		uint16_t iResolution,
		uint8_t iChannel,
		uint8_t iVoltageMin,
		uint8_t iVoltageMax
):
m_iResolution(iResolution),
m_iVoltageMin(iVoltageMin),
m_iVoltageMax(iVoltageMax)
{
	m_pMovingAverageFilter = new MovingAverageFilter(100,4);
	m_JS_1 = new JoyStick;
	m_JS_2 = new JoyStick;
	m_JS_1->Channel = iChannel;
	m_fConv = 0;
	analogReadResolution(m_iResolution);
	m_fConv = (float)( (float)((float)(1)/65536*5)  );
}

float QuadJoyStickDriver::GetVerticalVoltageFilter()
{
	return m_pMovingAverageFilter->process( (float)analogRead(m_JS_1->Channel)*m_fConv  );
}

float QuadJoyStickDriver::GetThrotleVoltage()
{
	return (float)analogRead(m_JS_1->Channel)*m_fConv ;
}


uint16_t QuadJoyStickDriver::GetVoltageMax()
{
	return m_iVoltageMax;
}

uint16_t QuadJoyStickDriver::GetVoltageMin()
{
	return m_iVoltageMin;
}


QuadJoyStickDriver::~QuadJoyStickDriver()
{
	// TODO Auto-generated destructor stub
}

