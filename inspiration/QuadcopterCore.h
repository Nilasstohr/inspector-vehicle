/*
 * QuadcopterCore.h
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#ifndef QUADCOPTERCORE_H_
#define QUADCOPTERCORE_H_

#include "QuadcopterWorker.h"
#include "QuadcopterController.h"
#include "SparkFunLSM9DS1.h"
#include "QuadCopterFeedBack.h"
#include "UIButton.h"
#include "QuadcopterVisual.h"
#include "QuadLog.h"
#include "QuadSpeedControllerDriver.h"
#include "QuadJoyStickDriver.h"

class CQuadcopterCore {
	public:
		CQuadcopterCore(
			CBlueToothInterface  *pCBlueToothInterface,
			LSM9DS1				 *pLSM9DS1,
			CQuadcopterVisual    *pQuadCopterVisual,
			CQuadLog 			 *pQuadLog,
			CQuadSpeedControllerDriver * pQuadSpeedControl,
			CQuadJoyStickDriver *pQuadJoyStick,
			LIDARLite          * pLIDARLite
		);

		virtual ~CQuadcopterCore();

		CQuadcopterWorker * GetQuadcopterWorker(){return m_pQuadcopterWorker;}

	private:
		CBlueToothInterface   *m_pCBlueToothInterface;

		CQuadcopterWorker     *m_pQuadcopterWorker;
		CQuadcopterController *m_pQuadcopterController;
		LSM9DS1				  *m_pLSM9DS1;
		CQuadCopterFeedBack	  *m_pQuadCopterFeedBack;
		CQuadcopterVisual     *m_pQuadcopterVisual;
		CQuadLog 			  *m_pQuadLog;
		CQuadSpeedControllerDriver * m_pQuadSpeedControl;
		CQuadJoyStickDriver *  m_pQuadJoyStick;
		LIDARLite           * m_pLIDARLite;
};

#endif /* QUADCOPTERCORE_H_ */
