/*
 * QuadcopterCore.cpp
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#include "QuadcopterCore.h"
#include "TeensyPins.h"

CQuadcopterCore::CQuadcopterCore(
		CBlueToothInterface 	 *pCBlueToothInterface,
		LSM9DS1				 	 *pLSM9DS1,
		CQuadcopterVisual    	 *pQuadCopterVisual,
		CQuadLog 			 	 *pQuadLog,
		CQuadSpeedControllerDriver * pQuadSpeedControl,
		CQuadJoyStickDriver *pQuadJoyStick,
		LIDARLite          * pLIDARLite
):
m_pCBlueToothInterface(pCBlueToothInterface),
m_pLSM9DS1(pLSM9DS1),
m_pQuadcopterVisual(pQuadCopterVisual),
m_pQuadLog(pQuadLog),
m_pQuadSpeedControl(pQuadSpeedControl),
m_pQuadJoyStick(pQuadJoyStick),
m_pLIDARLite(pLIDARLite)
{
	m_pQuadCopterFeedBack   = new CQuadCopterFeedBack(m_pLSM9DS1,m_pQuadcopterVisual);
	m_pQuadcopterController = new CQuadcopterController(
			m_pQuadCopterFeedBack,
			m_pQuadcopterVisual,
			m_pQuadLog,
			m_pQuadSpeedControl,
			m_pQuadJoyStick,
			m_pLIDARLite,
			TS
	);

	m_pQuadcopterWorker = new CQuadcopterWorker(m_pQuadcopterController,m_pQuadcopterVisual);
}

CQuadcopterCore::~CQuadcopterCore() {
	// TODO Auto-generated destructor stub
}

