/*
 * QuadcopterWorker.h
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#ifndef QUADCOPTERWORKER_H_
#define QUADCOPTERWORKER_H_

#include "QuadcopterController.h"
#include "QuadcopterVisual.h"
#include "SyncEvent.h"

class CQuadcopterWorker {

	public:



		enum EWorker
		{
			WORKER_COM = 0,
			WORKER_CONTROLLER,
			WORKER_SUB_CLASSES
		};


		CQuadcopterWorker(
			CQuadcopterController *pQuadcopterController,
			CQuadcopterVisual     *pQuadCopterVisual
		);
		virtual ~CQuadcopterWorker();

		void Open();

	private:

		void Run();
		void BlinkWorker();
		void Calibration();
		void GPS();
		void GetFeedBack();
		void WorkerController();
	    void WorkerSubClasses();
	    void SetCom(uint8_t iCase);
	    void WorkerCom();

		CQuadcopterController *m_pQuadcopterController;
		CQuadcopterVisual     *m_pQuadCopterVisual;

		CQuadcopterVisual::EInterFace m_eInterface;

		bool m_bOpen;

		EWorker m_eWorker;
};

#endif /* QUADCOPTERWORKER_H_ */
