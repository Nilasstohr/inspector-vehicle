/*
 * QuadcopterConfig.h
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#ifndef QUADCOPTERCONFIG_H_
#define QUADCOPTERCONFIG_H_

#include "QuadcopterCore.h"
#include "BlueToothInterface.h"
#include "UIButton.h"
#include "SparkFunLSM9DS1.h"
#include "QuadcopterVisual.h"
#include "QuadLog.h"
#include "QuadSpeedControllerDriver.h"
#include "QuadJoyStickDriver.h"
#include "MovingAverageFilter.h"
#include "LIDARLite.h"

class CQuadcopterConfig {
	public:
		CQuadcopterConfig();
		virtual ~CQuadcopterConfig();

		CQuadcopterCore * GetQuadcopterCore(){return m_pQuadcopterCore;}

	private:
		CBlueToothInterface *m_pCBlueToothInterface;
		CQuadcopterVisual   *m_pQuadcopterVisual;

		CQuadcopterCore 	*m_pQuadcopterCore;
		LSM9DS1				*m_pLSM9DS1;
		CQuadLog 				*m_pQuadLog;
		CQuadSpeedControllerDriver * m_QuadSpeedControl;
        CQuadJoyStickDriver * m_pQuadJoyStick;
        MovingAverageFilter* m_pMovingAverageFilter;
        LIDARLite          * m_pLIDARLite;
};

#endif /* QUADCOPTERCONFIG_H_ */
