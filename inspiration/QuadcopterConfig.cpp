/*
 * QuadcopterConfig.cpp
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#include "QuadcopterConfig.h"
#include "TeensyPins.h"

CQuadcopterConfig::CQuadcopterConfig()
{
	m_pCBlueToothInterface = new CBlueToothInterface;
	//m_pUIButton            = new CUIButton(PIN_DIGITAL_14);

	m_pQuadcopterVisual = new CQuadcopterVisual(m_pCBlueToothInterface);


	m_pLSM9DS1 = new LSM9DS1(IMU_MODE_SPI, LSM9DS1_AG_CS, LSM9DS1_M_CS);

	m_pQuadLog = new  CQuadLog(LOG_ITEM_CAP,LOG_ITEM_CAP_RAW,LOG_ITEM_CAP_CONTROL);

	m_QuadSpeedControl = new  CQuadSpeedControllerDriver
   (
		SPEED_CONTROLLER_PWM_FREQ,
		SPEED_CONTROLLER_PWM_RES,
		PIN_PWM_22,
		PIN_PWM_23,
		PIN_PWM_20,
		PIN_PWM_21
	);

	m_pMovingAverageFilter = new MovingAverageFilter(100,4);

	m_pQuadJoyStick = new CQuadJoyStickDriver(
			m_pMovingAverageFilter,
			16,
			A1,
			0,
			0,
			4
	);

	/*
	m_pQuadDisSensor = new CQuadDisSensor
	(
		DIS_SENSOR_A_EX_FIT,
		DIS_SERNSR_B_EX_FIX,
		DIS_SENSOR_ADC_RES,
		DIS_SENSOR_AVG_NUM,
		DIS_SENSOR_ADC_VOLTAGE_SCALE,
		DIS_SENSOR_PIN
	);
	*/

	m_pLIDARLite = new LIDARLite();


	m_pQuadcopterCore = new CQuadcopterCore(
		m_pCBlueToothInterface,
		m_pLSM9DS1,
		m_pQuadcopterVisual,
		m_pQuadLog,
		m_QuadSpeedControl,
		m_pQuadJoyStick,
		m_pLIDARLite
	);
}

CQuadcopterConfig::~CQuadcopterConfig()
{

}

