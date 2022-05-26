/*
 * QuadForceTorqueSensor.h
 *
 *  Created on: 11/01/2017
 *      Author: Nilasstohr
 */

#ifndef QUADFORCETORQUESENSOR_H_
#define QUADFORCETORQUESENSOR_H_

#include "core_pins.h"
#include "MovingAverageFilter.h"

class CQuadForceTorqueSensor {

	public:
	CQuadForceTorqueSensor(
		uint8_t slaveSelectPin,
		 uint8_t clkPin,
						 uint8_t MISOPin,
				  		 float resolution, //distance per discrete value in um
				  		 float Kspring, //N/um
				  		 uint16_t MovingAverageSize
	);

	virtual ~CQuadForceTorqueSensor();

	public:

	   bool Open();
	   void GetPosition1(uint16_t &DigitalPosition);
	   void GetForce(uint16_t Position, float &Force);
	   void GetStationaryPosition();
	   void PosCorrection(int32_t &ThrustPosCor,int32_t Thrustmes);
	   void InitPosCorrection();
	   void SetOffSet(uint16_t OffSet);
	private:
	   void initSPI();

	   uint8_t m_slaveSelectPin;
	   uint8_t m_clkPin;
	   uint8_t m_MISOPin;
	   uint8_t m_bOpen;

	   float m_resolution; //distance per discrete value in um
	   float m_Kspring; //N/um

	  	float m_force; //N
	  	float    m_delta_x ;
	  	uint16_t m_x_cur   ;
	  	uint16_t m_x_last1 ;
	    uint16_t m_x_last2 ;
	  	uint16_t m_stationary;
	  	uint16_t m_MovingAverageSize;
	  	uint16_t m_OffSet;

      	 MovingAverageFilter * m_pMovinAverageFilter;
//	  	 CQuadcopterVisual   * m_pQuadcopterVisual;

      	int16_t ThrustCur;
      	int16_t ThrustLast;
      	int16_t step;
      	int32_t diff;

};

#endif /* QUADFORCETORQUESENSOR_H_ */
