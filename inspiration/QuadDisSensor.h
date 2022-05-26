/*
 * QuadcopterCore.h
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#ifndef QUADDISSENSOR_H_
#define QUADDISSENSOR_H_

#include "core_pins.h"
#include <stdlib.h>
#include <usb_serial.h>
#include "TeensyPins.h"


class CQuadDisSensor {
	public:
		CQuadDisSensor(float a,float b,uint8_t Res, uint8_t AvgValue,float AdcVoltageScale, uint8_t Apin);

		virtual ~CQuadDisSensor();
		float GetDistance();
		float GetDistanceAvg();
		bool Open();

	private:
		float ReadDistanceVolt(uint8_t Sensor);
		void InitDisSensor();
		float ConvertVolt2Dis(float VoltDis);

		float m_a;
		float m_b;
		uint8_t m_Res;
		uint8_t m_AvgValue;
		uint8_t m_Apin;
		float m_AdcVoltageScale;


};

#endif /* QUADCOPTERCORE_H_ */
