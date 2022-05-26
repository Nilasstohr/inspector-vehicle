/*
 * QuadcopterController.h
 *
 *  Created on: 30/05/2015
 *      Author: Nilasstohr
 */

#ifndef QUADFORCETORQUE_H_
#define QUADFORCETORQUE_H_

#include "core_pins.h"
#include <stdlib.h>
#include "QuadcopterVisual.h"
#include "MovingAverageFilter.h"


////////////////////////////
// Register               //
////////////////////////////



class CQuadForceTorque {

	public:
		CQuadForceTorque
		(
			uint8_t slaveSelectPin,
			uint8_t clkPin,
			uint8_t MISOPin,
			uint8_t MovingAverageSize,
			float resolution,
			float Kspring,
			CQuadcopterVisual *pQuadcopterVisual
		);

		bool Open();
		void GetPosition1(uint16_t &DigitalPosition);
		void GetForce(uint16_t Position, float &Force);
		void GetStationaryPosition();

	private:
		void initSPI();
		bool Initialize();

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

		 MovingAverageFilter * m_pMovinAverageFilter;
		 CQuadcopterVisual   * m_pQuadcopterVisual;

};

#endif /* QUADCOPTERCONTROLLER_H_ */
