/*
 * QuadCopterFeedBack.h
 *
 *  Created on: 20/02/2016
 *      Author: Nilasstohr
 */

#ifndef QUADCOPTERFEEDBACK_H_
#define QUADCOPTERFEEDBACK_H_

#include "core_pins.h"
#include <stdlib.h>
#include "SparkFunLSM9DS1.h"
#include "QuadcopterVisual.h"

class CQuadCopterFeedBack {

	public:
		CQuadCopterFeedBack(LSM9DS1* pLSM9DS1,CQuadcopterVisual *pQuadCopterVisual);
		virtual ~CQuadCopterFeedBack();

		void Open();
		bool IsOpen(){ return m_bOpen;}
		bool GetConvertFactor(float &gRes, float &aRes);
		bool GetFeedback(float &pitch, float &roll, float &heading);
		bool GetBias(float* gBias, float * aBias);
		bool SetBias();
		bool Reset();
		void ResetComFilter();
		bool GetFeedbackComFilter(
			float &pitch,
			float &roll,
			float &fpitch,
			float &froll,
			float &heading,
			float gyroGain,
			float AccGain
		);
		bool GetFeedbackRaw(
				int16_t &xg,
				int16_t &yg,
				int16_t &zg,
				int16_t &xa,
				int16_t &ya,
				int16_t &za
		);

		void SetComFilterParams
		(
			float a1r,
			float b1r,
			float b0r,
			float taur,
			float kr,
			float a1p,
			float b1p,
			float b0p,
			float taup,
			float kp
		);


		void GetSampleRaw
		(
			float &lgx,
			float &lgy,
			float &lgz,
			float &lax,
			float &lay,
			float &laz
		);


	private:

		void UpdateGyro();
		void UpdateAccel();
		void UpdateMag();
		void HeadingCalc(float hx, float hy, float &heading);
		void OrientationCalc(float x, float y, float z, float &pitch, float &roll);
		void OrientationCalcComfilter(
			float xg,
			float yg,
			float zg,
			float xa,
			float ya,
			float za,
			float &fpitch,
			float &froll,
			float &pitch,
			float &roll
		);

		// Bias
		typedef struct _SBias
		{
			float xgb;
			float ygb;
			float zgb;
			float xab;
			float yab;
			float zab;
		}SBias;

		SBias *Bi;

		// Complementary filter
		typedef struct _SComFilter
		{
			float a1;
			float b1;
			float b0;
			float tau;
			float dummy1;
			float dummy2;
			float f1;
			float f0;
			float input;
			float KScale;
		}SComFilter;

		SComFilter *FP;
		SComFilter *FR;
		void InitComFilter();


		float m_lgx;
		float m_lgy;
		float m_lgz;
		float m_lax;
		float m_lay;
		float m_laz;

		float pa;
		float ra;


		bool m_bOpen;
		LSM9DS1* m_pLSM9DS1;
		CQuadcopterVisual   *m_pQuadcopterVisual;


};

#endif /* QUADCOPTERFEEDBACK_H_ */
