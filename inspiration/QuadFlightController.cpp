/*
 * QuadFlightController.cpp
 *
 *  Created on: 30/05/2015
 *      Author: Nilasstohr
 */
#include <stdio.h>
#include "QuadFlightController.h"
#include <usb_serial.h>

 
CQuadFlightController::CQuadFlightController(uint32_t iTransmitRate,float AdcVoltageScale, CQuadcopterVisual *pQuadcopterVisual):
m_iTransmitRate(iTransmitRate),
m_pQuadcopterVisual(pQuadcopterVisual),
m_bOpen(false),
m_AdcVoltageScale(AdcVoltageScale)
{
	m_pFlightControls = new SFlightControls;
	m_pFlightControls->Altitude =0;
}

CQuadFlightController::~CQuadFlightController()
{

}

bool CQuadFlightController::Open()
{
	if(Initialize())
	{
		m_bOpen = true;
	}
	return m_bOpen;
}

bool CQuadFlightController::Initialize()
{
	m_pFlightControls->Pitch    =0;
	m_pFlightControls->Roll     =0;
	m_pFlightControls->Altitude =0;
	m_pQuadcopterVisual->Open();
	analogReadResolution(16);

	pinMode(22, INPUT);
	pinMode(21, INPUT);
	pinMode(20, INPUT);

	m_ap = (10.0-0.0)/(5.0-2.5);
	m_bp = -m_ap*2.5;
	m_ar =  m_ap;
	m_br =  m_bp;

	return true;
}

void CQuadFlightController::Run()
{
	Open();
	while(1)
	{
		TransmitFlightControls();
		delayMicroseconds(m_iTransmitRate);
		//Serial.println();
		//delay(10);
	}
}

void CQuadFlightController::GetFlightControls(FlightControls * pFlightControls)
{
	pFlightControls->Pitch       = m_ap*ReadVolt(22) +m_bp;
	pFlightControls->Roll        = m_ar*ReadVolt(21) +m_br;
	//pFlightControls->Altitude    = ReadVolt(22);
}

float CQuadFlightController::ReadVolt(uint8_t Sensor)
{
  uint16_t sensorValue = analogRead(Sensor);
  float distance =  m_AdcVoltageScale*sensorValue;
  return distance;
}

void CQuadFlightController::FlightControls2Cmd(char * pCmd, uint16_t &iSize,FlightControls * pFlightControls)
{
	iSize = sprintf(pCmd,"FC:%f,%f,%f;",
		pFlightControls->Pitch,
		pFlightControls->Roll,
		pFlightControls->Altitude
	);
}

void CQuadFlightController::TransmitFlightControls()
{
	char Cmd[200];
	uint16_t iSize=0;
	GetFlightControls(m_pFlightControls);
	FlightControls2Cmd(Cmd,iSize,m_pFlightControls);
	m_pQuadcopterVisual->Write(Cmd,iSize,CQuadcopterVisual::INTERFACE_WIRE);
}


