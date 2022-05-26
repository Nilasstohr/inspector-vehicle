/*
 * QuadForceTorqueSensor.cpp
 *
 *  Created on: 11/01/2017
 *      Author: Nilasstohr
 */

#include "QuadForceTorqueSensor.h"
#include <SPI.h>  // SPI library is used for...SPI.
#include "TeensyPins.h"

CQuadForceTorqueSensor::CQuadForceTorqueSensor(
	uint8_t slaveSelectPin,
	uint8_t clkPin,
	uint8_t MISOPin,
	float resolution, //distance per discrete value in um
	float Kspring, //N/um
	uint16_t MovingAverageSize
):
m_slaveSelectPin(slaveSelectPin),
m_clkPin(clkPin),
m_MISOPin(MISOPin),
m_bOpen(false),
m_MovingAverageSize(MovingAverageSize),
m_resolution(resolution),
m_Kspring(Kspring),
ThrustCur(0),
ThrustLast(0),
step(0),
diff(0)
{
	m_pMovinAverageFilter = new MovingAverageFilter(m_MovingAverageSize,0);
}

CQuadForceTorqueSensor::~CQuadForceTorqueSensor() {
	// TODO Auto-generated destructor stub
}

bool CQuadForceTorqueSensor::Open()
{
	initSPI();
}

void CQuadForceTorqueSensor::initSPI()
{

	        pinMode(10,OUTPUT);
	        pinMode(14,OUTPUT);
	        pinMode(12,INPUT);

	        digitalWrite(10,HIGH);

	        //SPI.beginTransaction(SPISettings(freq,MSBFIRST,SPI_MODE3));

	    	SPI.setSCK(14);
	    	// Maximum SPI frequency is 10MHz, could divide by 2 here:
	    	SPI.setClockDivider(SPI_CLOCK_DIV16);
	    	// Data is read and written MSb first.
	    	SPI.setBitOrder(MSBFIRST);
	    	// Data is captured on rising edge of clock (CPHA = 0)
	    	// Base value of the clock is HIGH (CPOL = 1)
	    	SPI.setDataMode(SPI_MODE3);

	    	SPI.begin();

	        delay(2000);
}

void CQuadForceTorqueSensor::GetPosition1(uint16_t &DigitalPosition)
{
	digitalWrite(10,LOW);
	delay(0);
	byte byte1=SPI.transfer(0x00);
	byte byte2=SPI.transfer(0x00);
	byte byte3=SPI.transfer(0x00);
	digitalWrite(10,HIGH);
	delay(0.001);
	DigitalPosition = (byte1&0x7F)<<5 | byte2>>3;
	//stat = (byte2&0x07)<<3 | (byte3&0xE0)>>5;
	// Serial.println(pos);
}



void CQuadForceTorqueSensor::GetStationaryPosition()
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

void CQuadForceTorqueSensor::GetForce(uint16_t Position, float &Force)
{
	Force = (float)(Position*m_Kspring);
}

void CQuadForceTorqueSensor::InitPosCorrection()
{
	ThrustCur=0;
	ThrustLast=0;
	step=0;
	diff=0;
}
void CQuadForceTorqueSensor::PosCorrection(int32_t &ThrustPosCor,int32_t Thrustmes)
{
	ThrustCur=Thrustmes;

	diff = ThrustCur-ThrustLast;

	if(abs(diff) > 3800)
	{
	    if(diff > 0)
	    {
	        step = step - diff;
	    }
	    else
	    {
	       step = step + abs(diff);
	    }
	}
	ThrustPosCor =  Thrustmes + step; //- m_OffSet ;

	/*
	Serial.print(diff);
	Serial.print(" ");
	Serial.print(step);
	Serial.print(" ");
	Serial.print(Thrustmes);
	Serial.print(" ");
	Serial.print(ThrustPosCor);
	Serial.print(" ");
	Serial.println(m_OffSet);
	*/

	ThrustLast = ThrustCur;
}

void CQuadForceTorqueSensor::SetOffSet(uint16_t OffSet)
{
	m_OffSet = OffSet;
}
