/*
 * QuadCopterFeedBack.cpp
 *
 *  Created on: 20/02/2016
 *      Author: Nilasstohr
 */

#include "QuadCopterFeedBack.h"

CQuadCopterFeedBack::CQuadCopterFeedBack(LSM9DS1* pLSM9DS1,CQuadcopterVisual *pQuadCopterVisual):
m_bOpen(false),
m_pLSM9DS1(pLSM9DS1),
m_pQuadcopterVisual(pQuadCopterVisual)
{
	FP  = new _SComFilter;
	FR  = new _SComFilter;
	Bi  = new  _SBias;
	InitComFilter();
}

void CQuadCopterFeedBack::Open()
{
	if (!m_pLSM9DS1->begin())
	{
		while(1)
		{
			Serial.println("m_pLSM9DS1->begin() failed");
			delay(1000);
		}
	}
	else
	{
		m_bOpen = true;
		m_pLSM9DS1->calibrate(true);
	}
}

CQuadCopterFeedBack::~CQuadCopterFeedBack()
{

}

void CQuadCopterFeedBack::UpdateGyro()
{
	m_pLSM9DS1->readGyro();
}

void CQuadCopterFeedBack::UpdateAccel()
{
	m_pLSM9DS1->readAccel();
}

void CQuadCopterFeedBack::UpdateMag()
{
	m_pLSM9DS1->readMag();
}

void CQuadCopterFeedBack::HeadingCalc(float hx, float hy, float &heading)
{
  if (hy > 0)
  {
    heading = 90 - (atan(hx / hy) * (180 / PI));
  }
  else if (hy < 0)
  {
    heading = - (atan(hx / hy) * (180 / PI));
  }
  else // hy = 0
  {
    if (hx < 0) heading = 180;
    else heading = 0;
  }
}

void CQuadCopterFeedBack::OrientationCalc(float x, float y, float z, float &pitch, float &roll)
{
  pitch = atan2(x, sqrt(y * y) + (z * z));
  roll = atan2(y, sqrt(x * x) + (z * z));
  pitch *= 180.0 / PI;
  roll *= 180.0 / PI;
}

void CQuadCopterFeedBack::OrientationCalcComfilter(
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
)
{

/*
	Serial.print("  Gyro: ");
    Serial.print(xg);
	Serial.print(" ");
	Serial.print(yg);
	Serial.print(" ");
		Serial.print(zg);
		Serial.print(" ");
		Serial.print("  Acc: ");
		Serial.print(xa);
		Serial.print(" ");
		Serial.print(ya);
		Serial.print(" ");
		Serial.print(za);
		Serial.print("  P/R: ");

	pa = atan2(xa, sqrt(ya * ya) + (za * za));
    ra = atan2(ya, sqrt(xa * xa) + (za * za));
     pa *= 180.0 / PI;
     ra *= 180.0 / PI;

     Serial.print(" ");
     Serial.print(pa);
    Serial.print(" ");
     Serial.println(ra);

*/

Bi->xgb=0;
Bi->ygb=0;
Bi->zgb=0;
Bi->xab=0;
Bi->yab=0;
Bi->zab=0;

  xg = xg - Bi->xgb;
  yg = yg - Bi->ygb;
  zg = zg - Bi->zgb;
  xa = xa - Bi->xab;
  ya = ya - Bi->yab;
  za = za - Bi->zab;

/*
  Serial.print("  GyroB: ");
      Serial.print(xg);
  	Serial.print(" ");
  	Serial.print(yg);
  	Serial.print(" ");
  		Serial.print(zg);
  		Serial.print(" ");
  		Serial.print("  AccB: ");
  		Serial.print(xa);
  		Serial.print(" ");
  		Serial.print(ya);
  		Serial.print(" ");
  		Serial.print(za);
  		Serial.print("  P/R: ");
*/

  //pa = atan2(xa, sqrt(ya * ya) + (za * za));
  //ra = atan2(ya, sqrt(xa * xa) + (za * za));

  pa = atan2(ya, za);
  ra = atan2(-xa, sqrt(ya * ya + za * za));

  pa *= 180.0 / PI;
  ra *= 180.0 / PI;

//  Serial.print(" ");
//  Serial.print(pa);
//  Serial.print(" ");
//  Serial.println(ra);

  pitch = pa;
  roll  = ra;

  // filter pitch
  FP->input = pa -  FP->KScale*yg*FP->tau;

  FP->f0 =  FP->input - FP->dummy1;

  fpitch = FP->f0 * FP->b0 + FP->dummy2;

  FP->f1 =  FP->f0;

  FP->dummy1 =  FP->a1 * FP->f1;

  FP->dummy2 =  FP->b1 * FP->f1;


  // filter roll
  FR->input = ra + FR->KScale*xg*FR->tau;

  FR->f0 =  FR->input - FR->dummy1;

  froll = FR->f0*FR->b0 + FR->dummy2;

  FR->f1 =  FR->f0;

  FR->dummy1 =  FR->a1 * FR->f1;

  FR->dummy2 =  FR->b1 * FR->f1;

}

void CQuadCopterFeedBack::ResetComFilter()
{
		FP-> dummy1=0;
		FP-> dummy2=0;
		FP-> f1=0;
		FP-> f0=0;
		FP-> input=0;
		FR-> dummy1=0;
		FR-> dummy2=0;
		FR-> f1=0;
		FR-> f0=0;
		FR-> input=0;
}

void CQuadCopterFeedBack::InitComFilter()
{
	FP-> a1=-0.9983;
	FP-> b1=0.00083231;
	FP-> b0=0.00083231;
	FP-> tau=1;

	//FP-> a1=-0.9997;
	//FP-> b1= 0.00016657;
	//FP-> b0= 0.00016657;
	//FP-> tau=5;

	/*
	FP-> a1=-0.9998;
	FP-> b1=0.000083293;
	FP-> b0=0.000083293;
	FP-> tau=10;
	*/

	FP-> dummy1=0;
	FP-> dummy2=0;
	FP-> f1=0;
	FP-> f0=0;
	FP-> input=0;

	FR-> a1=-0.9983;
	FR-> b1= 0.00083231;
	FR-> b0= 0.00083231;
	FR-> tau=1;

	//FR-> a1=-0.9997;
	//FR-> b1= 0.00016657;
	//FR-> b0= 0.00016657;
	//FR-> tau=5;

	/*
	FR-> a1=-0.9998;
	FR-> b1= 0.000083293;
	FR-> b0= 0.000083293;
	FR-> tau=10;
	*/

	FR-> dummy1=0;
	FR-> dummy2=0;
	FR-> f1=0;
	FR-> f0=0;
	FR-> input=0;

	FR->KScale=1;
	FP->KScale=1;
}

void CQuadCopterFeedBack::SetComFilterParams
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
)
{

	//FP-> a1=-0.9983;
	//FP-> b1=0.00083231;
	//FP-> b0=0.00083231;
	//FP-> tau=1;

	//FP-> a1=-0.9997;
	//FP-> b1= 0.00016657;
	//FP-> b0= 0.00016657;
	//FP-> tau=5;

	//FP-> a1=-0.9998;
	//FP-> b1=0.000083293;
	//FP-> b0=0.000083293;
	//FP-> tau=10;

	FP-> a1= a1p;
	FP-> b1= b1p;
	FP-> b0= b0p;
	FP-> tau=taup;

	FP-> dummy1=0;
	FP-> dummy2=0;
	FP-> f1=0;
	FP-> f0=0;
	FP-> input=0;

	//FR-> a1=-0.9983;
	//FR-> b1= 0.00083231;
	//FR-> b0= 0.00083231;
	//FR-> tau=1;

	//FR-> a1=-0.9997;
	//FR-> b1= 0.00016657;
	//FR-> b0= 0.00016657;
	//FR-> tau=5;

	//FR-> a1=-0.9998;
	//FR-> b1= 0.000083293;
	//FR-> b0= 0.000083293;
	//FR-> tau=10;

	FR-> a1=a1r;
	FR-> b1=b1r;
	FR-> b0=b0r;
	FR-> tau= taur;

	FR-> dummy1=0;
	FR-> dummy2=0;
	FR-> f1=0;
	FR-> f0=0;
	FR-> input=0;







//	FR->KScale=1.6;
//	FP->KScale=1.6;

	FR->KScale=kr;
	FP->KScale=kp;
}

bool CQuadCopterFeedBack::Reset()
{
	ResetComFilter();
	Open();
	return m_bOpen;
}

bool CQuadCopterFeedBack::SetBias()
{

	if(m_bOpen)
	{
		float gBias[3];
		float aBias[3];

		//m_pLSM9DS1->calLSM9DS1(gBias,aBias);

		//m_pLSM9DS0->calLSM9DS0_2(gBias,aBias);
		//delay(1000);

/*
		Bi->xgb = -1.50;
		Bi->ygb = -1.16;
		Bi->zgb = -2.06;
		Bi->xab = -0.06;
		Bi->yab =  0.03;
		Bi->zab = -0.01;
*/


		//m_pQuadcopterVisual->Write("Calc Done",9,CQuadcopterVisual::INTERFACE_WIRE);


		Bi->xgb = gBias[0];
		Bi->ygb = gBias[1];
		Bi->zgb = gBias[2];

		Bi->xab = aBias[0];
		Bi->yab = aBias[1];
		Bi->zab = aBias[2];


		return true;
	}
	return false;
}

bool CQuadCopterFeedBack::GetFeedback(float &pitch, float &roll, float &heading)
{
	if(m_bOpen)
	{
		UpdateGyro();
		UpdateAccel();
		UpdateMag();

		HeadingCalc((float)m_pLSM9DS1->mx, (float)m_pLSM9DS1->my,heading);

		OrientationCalc
		(
			m_pLSM9DS1->calcAccel(m_pLSM9DS1->ax),
			m_pLSM9DS1->calcAccel(m_pLSM9DS1->ay),
			m_pLSM9DS1->calcAccel(m_pLSM9DS1->az),
			pitch,
			roll
		);
		return true;
	}
	return false;
}

bool CQuadCopterFeedBack::GetFeedbackComFilter(
	float &fpitch,
	float &froll,
	float &pitch,
	float &roll,
	float &heading,
	float gyroGain,
	float AccGain
)
{
	if(m_bOpen)
	{
		UpdateGyro();
		UpdateAccel();
		UpdateMag();

		HeadingCalc((float)m_pLSM9DS1->mx, (float)m_pLSM9DS1->my,heading);

		m_lgx = m_pLSM9DS1->calcGyro(m_pLSM9DS1->gx)*gyroGain;
		m_lgy = m_pLSM9DS1->calcGyro(m_pLSM9DS1->gy)*gyroGain;
		m_lgz = m_pLSM9DS1->calcGyro(m_pLSM9DS1->gz)*gyroGain;
		m_lax =	m_pLSM9DS1->calcAccel(m_pLSM9DS1->ax)*AccGain;
		m_lay =	m_pLSM9DS1->calcAccel(m_pLSM9DS1->ay)*AccGain;
		m_laz =	m_pLSM9DS1->calcAccel(m_pLSM9DS1->az)*AccGain;

		OrientationCalcComfilter
		(
			m_lgx,
			m_lgy,
			m_lgz,
			m_lax,
			m_lay ,
			m_laz,
			fpitch,
			froll,
			pitch,
			roll
		);
		return true;
	}
	return false;
}

void CQuadCopterFeedBack::GetSampleRaw(
			float &lgx,
			float &lgy,
			float &lgz,
			float &lax,
			float &lay,
			float &laz
)
{
	lgx=  m_lgx;
	lgy=  m_lgy;
	lgz=  m_lgz;
	lax=  m_lax;
	lay=  m_lay;
	laz=  m_laz;
}

bool CQuadCopterFeedBack::GetFeedbackRaw(
		int16_t &xg,
		int16_t &yg,
		int16_t &zg,
		int16_t &xa,
		int16_t &ya,
		int16_t &za
)
{
	if(m_bOpen)
	{
		UpdateGyro();
		UpdateAccel();
		UpdateMag();
		xg=m_pLSM9DS1->gx;
		yg=m_pLSM9DS1->gy;
		zg=m_pLSM9DS1->gz;
		xa=m_pLSM9DS1->ax;
		ya=m_pLSM9DS1->ay;
		za=m_pLSM9DS1->az;
		return true;
	}
	return false;
}

bool CQuadCopterFeedBack::GetConvertFactor(float &gRes, float &aRes)
{
	if(m_bOpen)
	{
		//gRes = m_pLSM9DS1->gRes;
		//aRes = m_pLSM9DS1->aRes;
		return true;
	}
	return false;
}

bool CQuadCopterFeedBack::GetBias(float* gBias, float * aBias)
{
	if(m_bOpen)
	{
		gBias[0]= Bi->xgb;
		gBias[1]= Bi->ygb;
		gBias[2]= Bi->zgb;

		aBias[0]= Bi->xab;
		aBias[1]= Bi->yab;
		aBias[2]= Bi->zab;

		return true;
	}
	return false;
}

