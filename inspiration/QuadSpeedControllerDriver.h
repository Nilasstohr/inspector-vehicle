/*
 * CSpeedControllerDriver.h
 *
 *  Created on: 30/05/2019
 *      Author: nist
 */

#ifndef QUADSPEEDCONTROLLERDRIVER_H_
#define QUADSPEEDCONTROLLERDRIVER_H_

#include "core_pins.h"
#include <stdlib.h>

class CQuadSpeedControllerDriver {

	public:
		CQuadSpeedControllerDriver(
			uint16_t iFreqPWM,
			uint16_t iResolution,
			uint8_t iChannel1,
			uint8_t iChannel2,
			uint8_t iChannel3,
			uint8_t iChannel4
		);

		void init();
		void SetSpeedBin(
			uint16_t iSpeedbin_SC1,
			uint16_t iSpeedbin_SC2,
			uint16_t iSpeedbin_SC3,
			uint16_t iSpeedbin_SC4);

		uint16_t GetSpeedBinMax();
		uint16_t GetSpeedBinMin();

		void SetSpeedControlleroff();


		void ThrotleTest();

		typedef struct sSpeedController
		{
			float Channel;
			float SpeedPWM;
			float MaxAllowedbin;
			float MinAllowedbin;


		}SpeedController;

		SpeedController  *m_SC1;
		SpeedController  *m_SC2;
		SpeedController  *m_SC3;
		SpeedController  *m_SC4;

	private:

		float Ms2Pwm(float ms);
		void CheckSCSaturration(uint16_t &iSpeedbin_SC1, uint16_t &iSpeedbin_SC2, uint16_t &iSpeedbin_SC3, uint16_t &iSpeedbin_SC4);
		uint16_t m_iFreqPWM;
		uint16_t m_iResolution;
		uint16_t m_iMaxDutyCycle_ms;
		uint16_t m_iMinDutyCycle_ms;
		uint16_t m_iStepValue_ms;
		uint16_t m_iStepValue_bin;
		uint16_t m_iStep_ms;
		uint16_t m_iMaxDutyCycle_bin;
		uint16_t m_iMinDutyCycle_bin;

	virtual ~CQuadSpeedControllerDriver();
};

#endif /* QUADSPEEDCONTROLLERDRIVER_H_ */
