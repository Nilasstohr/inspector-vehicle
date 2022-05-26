/*
 * QuadFlightController.h
 *
 *  Created on: 30/05/2015
 *      Author: Nilasstohr
 */

#ifndef QUADFLIGHTCONTROLLER_H_
#define QUADFLIGHTCONTROLLER_H_

#include "core_pins.h"
#include "QuadcopterVisual.h"


class CQuadFlightController
{


	public:
	
		CQuadFlightController(uint32_t iTransmitRate,float AdcVoltageScale, CQuadcopterVisual *pQuadcopterVisual);
		
		virtual ~CQuadFlightController();

		typedef struct SFlightControls
		{
			float Pitch;
			float Roll;
			float Altitude;
		}FlightControls;

		void Run();
	
	private:
		
		void FlightControls2Cmd(char *pCmd, uint16_t &iSize,FlightControls * pFlightControls);
		void GetFlightControls(FlightControls * pFlightControls);
		void TransmitFlightControls();
		float ReadVolt(uint8_t Sensor);
		bool Initialize();
		bool Open();
		
		uint32_t           m_iTransmitRate;
		FlightControls    *m_pFlightControls;
		CQuadcopterVisual *m_pQuadcopterVisual;
		
		bool m_bOpen;
		float m_AdcVoltageScale;
		float m_ap;
		float m_bp;
		float m_ar;
		float m_br;
	
};

#endif /* QUADCOPTERCONTROLLER_H_ */
