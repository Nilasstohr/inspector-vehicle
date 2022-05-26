/*
 * QuadForceTorque.cpp
 *
 *  Created on: 30/05/2015
 *      Author: Nilasstohr
 */

#include "QuadForceTorque.h"
#include "SyncEvent.h"
#include <stdio.h>
#include "SPI.h"


//#define CONTROLLER_DEBUG

CQuadForceTorque::CQuadForceTorque
(
	uint8_t slaveSelectPin,
	uint8_t clkPin,
	uint8_t MISOPin,
	uint8_t MovingAverageSize,
	float resolution,
	float Kspring,
	CQuadcopterVisual *pQuadcopterVisual
):
m_slaveSelectPin(slaveSelectPin),
m_clkPin(clkPin),
m_MISOPin(MISOPin),
m_bOpen(false),
m_MovingAverageSize(MovingAverageSize),
m_resolution(resolution),
m_Kspring(Kspring),
m_pQuadcopterVisual(pQuadcopterVisual)
{
	m_pMovinAverageFilter = new MovingAverageFilter(m_MovingAverageSize,0);
}

bool CQuadForceTorque::Open()
{
	if(Initialize())
	{
		m_bOpen = true;
	}
	return m_bOpen;
}

bool CQuadForceTorque::Initialize()
{

	delay(5000);
	Serial.println("Initilization begin");
	initSPI();

	/*
	GetStationaryPosition();

	m_delta_x  = 0;
    m_x_cur    = m_stationary ;
	m_x_last1  = m_stationary;
	m_x_last2  = m_stationary;
	m_force    = 0;
	return true;
	*/


}

void CQuadForceTorque::initSPI()
{
	pinMode(m_slaveSelectPin,OUTPUT);
	pinMode(m_clkPin,OUTPUT);
	pinMode(m_MISOPin,INPUT);
	digitalWrite(m_slaveSelectPin,HIGH);

	SPI.setSCK(14);
	// Maximum SPI frequency is 10MHz, could divide by 2 here:
	SPI.setClockDivider(SPI_CLOCK_DIV16);
	// Data is read and written MSb first.
	SPI.setBitOrder(MSBFIRST);
	// Data is captured on rising edge of clock (CPHA = 0)
	// Base value of the clock is HIGH (CPOL = 1)
	SPI.setDataMode(SPI_MODE3);

	SPI.begin();
}

void CQuadForceTorque::GetPosition1(uint16_t &DigitalPosition)
{
	digitalWrite(m_slaveSelectPin,LOW);
	byte byte1=SPI.transfer(0x00);
	byte byte2=SPI.transfer(0x00);
	//byte byte3=SPI.transfer(0x00);
	digitalWrite(m_slaveSelectPin,HIGH);
	delay(0.001);
	//delayMicroseconds(1);
	DigitalPosition = (byte1&0x7F)<<5 | byte2>>3;
	//stat = (byte2&0x07)<<3 | (byte3&0xE0)>>5;
}

void CQuadForceTorque::GetStationaryPosition()
{
	uint16_t DigitalPosition = 0;
	for(uint16_t i=0; i<m_MovingAverageSize; i++)
	{
		GetPosition1(DigitalPosition);
		m_pMovinAverageFilter->process(DigitalPosition);
	}
	GetPosition1(DigitalPosition);
	m_stationary = m_pMovinAverageFilter->process(DigitalPosition);
}


void CQuadForceTorque::GetForce(uint16_t Position, float &Force)
{
	Force = (float)(Position*m_Kspring);
}
