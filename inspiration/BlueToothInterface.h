/*
 * BlueToothInterface.h
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#ifndef BLUETOOTHINTERFACE_H_
#define BLUETOOTHINTERFACE_H_

#include <stdlib.h>
#include "core_pins.h"

class CBlueToothInterface {
	public:

		enum BTInterfaceMode
		{
			BT_ADMIND_MODE,
			BT_NORMAL_MODE
		};

		enum BTBaudRate
		{
			BT_BAUDRATE_115200=115200,
			BR9600=9600
		};


		CBlueToothInterface();
		bool Open(uint32_t iBaudRate, BTInterfaceMode Mode);

		void EmergencyStop();
		void Read(char * Message, uint8_t &iReadSize);
		void ReadFast(char * pMessage, uint8_t &iReadSize);
		void Write(const char * Message, uint8_t iWriteSize);
		void Echo();
		bool ToInt(long int &iValue);
		bool WriteGSP();
		void ReadGSP();
		void WriteFeedback(float pitch, float roll, float heading);

		virtual ~CBlueToothInterface();
	private:

		bool EnterConfigurationMode(char *pConfCommand);
		bool m_bOpen;
		uint32_t m_iBaudRate;

};

#endif /* BLUETOOTHINTERFACE_H_ */
