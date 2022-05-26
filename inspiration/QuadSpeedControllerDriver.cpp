/*
 * CSpeedControllerDriver.cpp
 *
 *  Created on: 30/05/2019
 *      Author: nist
 */

#include "QuadSpeedControllerDriver.h"
#include "TeensyPins.h"
#include "serial_com.h"

CQuadSpeedControllerDriver::CQuadSpeedControllerDriver
(
	uint16_t iFreqPWM,
	uint16_t iResolution,
	uint8_t iChannel1,
	uint8_t iChannel2,
	uint8_t iChannel3,
	uint8_t iChannel4
):
m_iFreqPWM(iFreqPWM),
m_iResolution(iResolution)
{
	m_SC1 = new SpeedController;
	m_SC2 = new SpeedController;
	m_SC3 = new SpeedController;
	m_SC4 = new SpeedController;

	m_SC1->Channel =iChannel1;
	m_SC2->Channel =iChannel2;
	m_SC3->Channel =iChannel3;
	m_SC4->Channel =iChannel4;

	//init();
}

void CQuadSpeedControllerDriver::init()
{
	pinMode(m_SC1->Channel , OUTPUT);   // sets the pin as output
	pinMode(m_SC2->Channel , OUTPUT);   // sets the pin as output
	pinMode(m_SC3->Channel , OUTPUT);   // sets the pin as output
	pinMode(m_SC4->Channel , OUTPUT);

	analogWriteRes(m_iResolution);

	analogWriteFrequency(m_SC1->Channel , m_iFreqPWM);
	analogWriteFrequency(m_SC2->Channel , m_iFreqPWM);
	analogWriteFrequency(m_SC3->Channel , m_iFreqPWM);
	analogWriteFrequency(m_SC4->Channel , m_iFreqPWM);

	float Ts_ms = 1/(float)(m_iFreqPWM)*1000;

	m_iMinDutyCycle_bin = (uint16_t)(1/Ts_ms*65536);
	m_iMaxDutyCycle_bin = (uint16_t)(2/Ts_ms*65536);

	m_iStepValue_bin = (m_iMaxDutyCycle_bin -m_iMinDutyCycle_bin)*0.1;

	analogWrite(m_SC1->Channel , m_iMinDutyCycle_bin);
	analogWrite(m_SC2->Channel ,  m_iMinDutyCycle_bin);
	analogWrite(m_SC3->Channel , m_iMinDutyCycle_bin);
	analogWrite(m_SC4->Channel ,  m_iMinDutyCycle_bin);

	m_SC1->MaxAllowedbin =25000;
	m_SC2->MaxAllowedbin =25000;
	m_SC3->MaxAllowedbin =25000;
	m_SC4->MaxAllowedbin =25000;

	m_SC1->MinAllowedbin =m_iMinDutyCycle_bin;
	m_SC2->MinAllowedbin =m_iMinDutyCycle_bin;
	m_SC3->MinAllowedbin =m_iMinDutyCycle_bin;
	m_SC4->MinAllowedbin =m_iMinDutyCycle_bin;


}

void CQuadSpeedControllerDriver::SetSpeedBin(
		uint16_t iSpeedbin_SC1,
		uint16_t iSpeedbin_SC2,
		uint16_t iSpeedbin_SC3,
		uint16_t iSpeedbin_SC4)
{
	CheckSCSaturration(iSpeedbin_SC1,iSpeedbin_SC2,iSpeedbin_SC3,iSpeedbin_SC3);

	analogWrite(m_SC1->Channel, iSpeedbin_SC1);  // analogRead values values from 0 to 255
	analogWrite(m_SC2->Channel, iSpeedbin_SC2);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
	analogWrite(m_SC3->Channel, iSpeedbin_SC3);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
	analogWrite(m_SC4->Channel, iSpeedbin_SC4);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
}

void CQuadSpeedControllerDriver::CheckSCSaturration(uint16_t &iSpeedbin_SC1, uint16_t &iSpeedbin_SC2, uint16_t &iSpeedbin_SC3, uint16_t &iSpeedbin_SC4)
{

	if(iSpeedbin_SC1 > m_SC1->MaxAllowedbin)
	{
		iSpeedbin_SC1 = m_SC1->MaxAllowedbin;
	}
	else if(iSpeedbin_SC1 < m_SC1->MinAllowedbin)
	{
		iSpeedbin_SC1= m_SC1->MinAllowedbin;
	}

	if(iSpeedbin_SC2 > m_SC2->MaxAllowedbin)
	{
		iSpeedbin_SC2 = m_SC2->MaxAllowedbin;
	}
	else if(iSpeedbin_SC2 < m_SC2->MinAllowedbin)
	{
		iSpeedbin_SC2= m_SC2->MinAllowedbin;
	}

	if(iSpeedbin_SC3 > m_SC3->MaxAllowedbin)
	{
		iSpeedbin_SC3 = m_SC3->MaxAllowedbin;
	}
	else if(iSpeedbin_SC3 < m_SC3->MinAllowedbin)
	{
		iSpeedbin_SC3= m_SC3->MinAllowedbin;
	}


	if(iSpeedbin_SC4 > m_SC4->MaxAllowedbin)
	{
		iSpeedbin_SC4 = m_SC4->MaxAllowedbin;
	}
	else if(iSpeedbin_SC4 < m_SC4->MinAllowedbin)
	{
		iSpeedbin_SC4= m_SC4->MinAllowedbin;
	}

}

void CQuadSpeedControllerDriver::ThrotleTest()
{
	uint16_t SpeedMaxAllowed = 25000;
	uint16_t Speed = m_iMinDutyCycle_bin;

	while(1)
	{
		Serial.print("Speed Testing:");
		Serial.println(Speed);

		SetSpeedBin(m_iMinDutyCycle_bin,m_iMinDutyCycle_bin,Speed,Speed);
		Speed +=500;

		if(Speed>SpeedMaxAllowed)
		{
			SetSpeedControlleroff();
			Serial.println("Done");
			return;
		}
		delay(2000);
	}
}

uint16_t CQuadSpeedControllerDriver::GetSpeedBinMax()
{
	return m_iMaxDutyCycle_bin;
}

uint16_t CQuadSpeedControllerDriver::GetSpeedBinMin()
{
	return m_iMinDutyCycle_bin;
}

float CQuadSpeedControllerDriver::Ms2Pwm(float ms)
{
	float PWM =  ms*m_iMinDutyCycle_bin;
	if(PWM > m_iMaxDutyCycle_bin)
	{
		PWM  = m_iMaxDutyCycle_bin;
	}
	else if(PWM < m_iMinDutyCycle_bin)
	{
		PWM = m_iMinDutyCycle_bin;
	}
	return PWM;
}

void CQuadSpeedControllerDriver::SetSpeedControlleroff()
{
	SetSpeedBin(
		m_iMinDutyCycle_bin,
		m_iMinDutyCycle_bin,
		m_iMinDutyCycle_bin,
		m_iMinDutyCycle_bin);
}


CQuadSpeedControllerDriver::~CQuadSpeedControllerDriver() {
	// TODO Auto-generated destructor stub
}

