/*
 * QuadRPMSensor.cpp
 *
 *  Created on: 26/01/2017
 *      Author: Nilasstohr
 */

#include "QuadRPMSensor.h"
#include <stdio.h>
#include <usb_serial.h>
#include "TeensyPins.h"

CQuadRPMSensor::CQuadRPMSensor(uint8_t AnalogPort,uint8_t AverageNum):
m_AnalogPort(AnalogPort),
m_stages(0),
m_PreviousTime(0),
m_CurrentTime(0),
m_sensorSUM(0),
m_AverageNum(AverageNum),
m_sensorAverage(0),
m_Threshold(12000),
m_RPM(0)
{


}

CQuadRPMSensor::~CQuadRPMSensor()
{

}


bool CQuadRPMSensor::Open()
{
	pinMode(m_AnalogPort, INPUT);
	return true;
}

void CQuadRPMSensor::ReadVoltagePort(float &Voltage)
{
	Voltage = analogRead(m_AnalogPort)*DIS_SENSOR_ADC_VOLTAGE_SCALE;
}

void CQuadRPMSensor::ReadDigPort(uint16_t &DigVoltage)
{
	DigVoltage = analogRead(m_AnalogPort);
}


void CQuadRPMSensor::ReadAvgVoltagePort(float &AvgVoltage)
{
	for(int i=0; i <m_AverageNum; i++)
	{
		m_sensorAverage = analogRead(m_AnalogPort);
		m_sensorSUM  += m_sensorAverage;
	}

	m_sensorAverage = m_sensorSUM/ m_AverageNum;
	m_sensorSUM=0;

	AvgVoltage = m_sensorAverage*DIS_SENSOR_ADC_VOLTAGE_SCALE;
}

void CQuadRPMSensor::ReadDigAvgVoltagePort(uint16_t &DigAvgVoltage)
{
    for(int i=0; i<20; i++)
	{
		m_sensorAverage = analogRead(m_AnalogPort);
		m_sensorSUM  += m_sensorAverage;
	}

	m_sensorAverage = m_sensorSUM/ 20;
	m_sensorSUM=0;

	DigAvgVoltage = m_sensorAverage;

}

void CQuadRPMSensor::GetRPM(float &RPM)
{

	// read the value from the sensor:
	//ReadVoltagePort(m_Voltage);

	//Serial.println(m_Voltage);
	ReadDigAvgVoltagePort(m_DigVoltage);

	switch(m_stages)
	{
		case 0:
		{
			//Serial.print("case 0 ");
			//Serial.print(" ");
			//Serial.print(m_Voltage);
			//Serial.print(" ");
			//Serial.println(m_Threshold);

			if(m_DigVoltage < m_Threshold)
			{

				m_PreviousTime = micros();
				m_stages = 1;
			}
			break;
		}
		case 1:
		{
			if(m_DigVoltage > m_Threshold)
			{
				m_stages = 2;
			}
			break;
		}
		case 2:
		{
			if(m_DigVoltage < m_Threshold)
			{
				m_CurrentTime = micros();

				m_RPM  =(float)(1/(2*( m_CurrentTime-m_PreviousTime)/1000000.0/60.0));

				//Serial.println(m_RPM);
				m_PreviousTime = m_CurrentTime;
				m_stages = 1;
			}
			break;
		}
	}
	 RPM = m_RPM;
}

