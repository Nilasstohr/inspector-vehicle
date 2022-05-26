/*
 * QuadcopterCore.cpp
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#include "QuadDisSensor.h"
#include "math.h"

CQuadDisSensor::CQuadDisSensor(float a,float b,uint8_t Res, uint8_t AvgValue,float AdcVoltageScale, uint8_t Apin):
m_a(a),
m_b(b),
m_Res(Res),
m_AvgValue(AvgValue),
m_Apin(Apin),
m_AdcVoltageScale(AdcVoltageScale)
{

}

CQuadDisSensor::~CQuadDisSensor()
{

}

bool CQuadDisSensor::Open()
{
	analogReadResolution(m_Res);
	return true;
}

float CQuadDisSensor::GetDistanceAvg()
{
  float SensorAvg=0;
  float SensorSum=0;
  float voltDis=0;

  for(int i=0; i<m_AvgValue; i++)
  {
     voltDis = ReadDistanceVolt(m_Apin);
     SensorSum += voltDis;
     delayMicroseconds(1);
  }
  SensorAvg = SensorSum/m_AvgValue;
  return ConvertVolt2Dis(SensorAvg);
  //return SensorAvg;
}

void CQuadDisSensor::InitDisSensor()
{
	analogReadResolution(16);
}

float CQuadDisSensor::GetDistance()
{
  float voltDis = ReadDistanceVolt(m_Apin);
  return ConvertVolt2Dis(voltDis);
}

float CQuadDisSensor::ReadDistanceVolt(uint8_t Sensor)
{
  uint16_t sensorValue = analogRead(Sensor);
  float distance =  m_AdcVoltageScale*sensorValue;
  return distance;
}

float CQuadDisSensor::ConvertVolt2Dis(float VoltDis)
{
  return log(VoltDis/m_a)/m_b;
}
