/*
 * QuadRPMSensor.h
 *
 *  Created on: 26/01/2017
 *      Author: Nilasstohr
 */


#ifndef QUADRPMSENSOR_H_
#define QUADRPMSENSOR_H_

#include "core_pins.h"
#include <stdlib.h>

class CQuadRPMSensor {

	public:
		CQuadRPMSensor(uint8_t AnalogPort,uint8_t AverageNum);
		bool Open();
		void GetRPM(float &RPM);
		void ReadVoltagePort(float &Voltage);
		void ReadAvgVoltagePort(float &AvgVoltage);
		void ReadDigPort(uint16_t &DigVoltage);
		void ReadDigAvgVoltagePort(uint16_t &DigAvgVoltage);

		virtual ~CQuadRPMSensor();

	private:
		uint8_t m_AnalogPort;
		uint8_t m_stages;

		int  m_PreviousTime;
		int  m_CurrentTime;
		uint32_t m_sensorSUM;
		uint8_t  m_AverageNum;
		uint32_t    m_sensorAverage;
		uint16_t m_Threshold;
		float m_RPM;
		float m_Voltage;
		uint16_t m_DigVoltage;
};

#endif /* QUADRPMSENSOR_H_ */
