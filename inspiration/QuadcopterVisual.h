/*
 * HALVisual.h
 *
 *  Created on: 02/10/2015
 *      Author: Nilasstohr
 */

#ifndef QUADCOPTERVISUAL_H_
#define QUADCOPTERVISUAL_H_

#include "BlueToothInterface.h"
#include "SyncEvent.h"

class CQuadcopterVisual
{
	public:

		enum EVisual
		{
			VISUAL_COM =0,
			ECHO_COM
		};

		enum EInterFace
		{
			INTERFACE_BT=0,
		    INTERFACE_WIRE,
		};


		CQuadcopterVisual(
				CBlueToothInterface *pBlueToothInterface
		);

		virtual ~CQuadcopterVisual();

		bool Open();
		void Run();
		void EchoTest(EInterFace eInterface);
		bool Com(uint8_t &iCase,EInterFace eInterface);
		void Read(char * pMessage, uint8_t &iReadSize, EInterFace eInterface);
		void ReadFast(char * pMessage, uint8_t &iReadSize,EInterFace eInterface);
		void Write(const char * pMessage, uint8_t iWriteSize,EInterFace eInterface);
		void Ack(EInterFace eInterface);
		void EmergencyStop();
		void ReadEmergencyStop();

		void WireRead(char * pMessage, uint8_t &iReadSize);
		void WireReadFast(char * pMessage, uint8_t &iReadSize);
		void WireWrite(const char * pMessage, uint8_t iWriteSize);
		void Echo();
		bool FecthArgs(char *pMessage,uint8_t &iReadSize, uint32_t* Arg,uint8_t &Argnum);
		bool FecthArgsFloat(char *pMessage,uint8_t &iReadSize, float * Arg,uint8_t &Argnum);
		bool Validate(char * &pMessage,uint8_t &iReadSize,const char * pValType);

		CSyncEvent1<uint8_t> SetHALMode;

	private:

		CBlueToothInterface *m_pBlueToothInterface;
		bool bOpen;
		EVisual m_eVisual;

};

#endif /* HALVISUAL_H_ */
