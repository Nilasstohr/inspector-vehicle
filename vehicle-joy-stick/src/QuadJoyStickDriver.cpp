/*
 * QuadJoyStickDriver.cpp
 *
 *  Created on: 31/05/2019
 *      Author: nist
 */

#include "QuadJoyStickDriver.h"

QuadJoyStickDriver::QuadJoyStickDriver(
		MovingAverageFilter * pMovingAverageFilter,
		uint16_t iResolution,
		uint8_t iChannel1,
		uint8_t iChannel2,
		uint8_t iVoltageMin,
		uint8_t iVoltageMax
):
m_pMovingAverageFilter(pMovingAverageFilter),
m_iResolution(iResolution),
m_iVoltageMin(iVoltageMin),
m_iVoltageMax(iVoltageMax)
{
	m_JS_1 = new JoyStick;
	m_JS_1->Channel = iChannel1;
	m_fConv = 0;
}

void QuadJoyStickDriver::init()
{
	analogReadResolution(m_iResolution);
	m_fConv = (float)( (float)((float)(1)/65536*5)  );
}


float QuadJoyStickDriver::GetThrotleVoltageFilter()
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

