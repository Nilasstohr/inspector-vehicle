/*
 * HALVisual.cpp
 *
 *  Created on: 02/10/2015
 *      Author: Nilasstohr
 */

#include "QuadcopterVisual.h"
#include "core_pins.h"
#include "WString.h"
#include "serial_com.h"
#include "TeensyPins.h"

#define COM_S "COM"

CQuadcopterVisual::CQuadcopterVisual(
		CBlueToothInterface *pBlueToothInterface
):
m_pBlueToothInterface(pBlueToothInterface),
bOpen(false)
{
	m_eVisual=ECHO_COM;
}

CQuadcopterVisual::~CQuadcopterVisual()
{

}

bool CQuadcopterVisual::Open()
{
	//m_pBlueToothInterface->Open(115200,CBlueToothInterface::BT_ADMIND_MODE);
	m_pBlueToothInterface->Open(115200,CBlueToothInterface::BT_NORMAL_MODE);
	Serial.begin(115200);
	bOpen=true;
	return true;
}

void CQuadcopterVisual::Run()
{

}

bool CQuadcopterVisual::Com(uint8_t &iCase,EInterFace eInterface)
{
	char aMessage[20];
	uint8_t iReadSize;
	uint8_t iArg=0;
	uint32_t aArg[1];

	Read(aMessage,iReadSize,eInterface);

	if(iReadSize>3)
	{
		char *pMessage = aMessage;
		if(Validate(pMessage,iReadSize,COM_S))
		{
			if(FecthArgs(pMessage,iReadSize,aArg,iArg))
			{
				if(iArg==1)
				{
					iCase=aArg[0];
					return true;
				}
			}
		}
	}
	return false;
}

void CQuadcopterVisual::Read(char * pMessage, uint8_t &iReadSize,EInterFace eInterface)
{
	if(eInterface==INTERFACE_BT)
	{
		m_pBlueToothInterface->Read(pMessage,iReadSize);
	}
	else
	{
		WireRead(pMessage,iReadSize);
	}
}

void CQuadcopterVisual::ReadFast(char * pMessage, uint8_t &iReadSize,EInterFace eInterface)
{
	if(eInterface==INTERFACE_BT)
	{
		m_pBlueToothInterface->ReadFast(pMessage,iReadSize);
	}
	else
	{
		WireReadFast(pMessage,iReadSize);
	}
}


void CQuadcopterVisual::Write(const char * pMessage, uint8_t iWriteSize,EInterFace eInterface)
{
	if(eInterface==INTERFACE_BT)
	{
		m_pBlueToothInterface->Write(pMessage,iWriteSize);
	}
	else
	{
		WireWrite(pMessage,iWriteSize);
	}
}

void CQuadcopterVisual::Ack(EInterFace eInterface)
{
	Write("ACKS",4,eInterface);
}

void CQuadcopterVisual::EchoTest(EInterFace eInterface)
{
	if(eInterface==INTERFACE_BT)
	{
		m_pBlueToothInterface->Echo();
	}
	else
	{
		Echo();
	}
}

void CQuadcopterVisual::WireRead(char * pMessage, uint8_t &iReadSize)
{
	uint8_t Size=0;
	bool bRead=true;

	while(bRead)
	{
		while(Serial.available())
		{
			pMessage[Size] = Serial.read();
			//Serial.print(pMessage[Size]);
			Size++;
			if(pMessage[Size-1]==';')
			{
				bRead =false;
				iReadSize = Size;
			}
			// emergency stop
			else if(pMessage[Size-1]=='z')
			{
				EmergencyStop();
				bRead =false;
				iReadSize = 1;
			}
		}
	}
	/*
	while(Serial.available())
	{
		pMessage[Size] = Serial.read();
		Size++;
	}
	iReadSize = Size;
	*/
}
void CQuadcopterVisual::WireReadFast(char * pMessage, uint8_t &iReadSize)
{
	uint8_t Size=0;

	while(Serial.available())
	{
		pMessage[Size] = Serial.read();
		Size++;
	}
	iReadSize = Size;
}

void CQuadcopterVisual::EmergencyStop()
{
	ReadEmergencyStop();
}

void CQuadcopterVisual::ReadEmergencyStop()
{
	/*
	uint16_t Speed = PWM_FREQ*0.001*65536;
	analogWrite(PIN_PWM_21, Speed);  // analogRead values values from 0 to 255
	analogWrite(PIN_PWM_20, Speed);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
	analogWrite(PIN_PWM_5,  Speed);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
	analogWrite(PIN_PWM_6,  Speed);
	*/
	char pMessage[10];
	if(Serial.available())
	{
		pMessage[0] = Serial.read();
		if(pMessage[0]=='z')
		{

			//analogWrite(PIN_PWM_21, 19000);  // analogRead values values from 0 to 255
			//analogWrite(PIN_PWM_20, 19000);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
			//analogWrite(PIN_PWM_5,  19000);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
			//analogWrite(PIN_PWM_6,  19000);
			//while(1){delay(1000);};
		}
	}
}

void CQuadcopterVisual::WireWrite(const char * pMessage, uint8_t iWriteSize)
{
	for(uint8_t i=0; i<iWriteSize; i++)
	{
		Serial.print(pMessage[i]);
	}
	delay(10);
}

void CQuadcopterVisual::Echo()
{
	char aMessage[100];
	uint8_t iReadSize=0;
	WireRead(aMessage,iReadSize);
	WireWrite(aMessage,iReadSize);
}

bool CQuadcopterVisual::FecthArgs(char *pMessage,uint8_t &iReadSize, uint32_t* Arg,uint8_t &Argnum)
{
	//Serial.print("FecthArgs:");
	//Serial.println(*pMessage);

	if(*pMessage==':')
	{
		uint8_t index=0;
		String s = ++pMessage;
		Argnum = 0;

		//Serial.print("iReadSize:");
		//Serial.println(iReadSize);
		for(uint8_t i=0; i<iReadSize;i++)
		{
			//Serial.print("--LOOP:");
			//Serial.print(i);
			//Serial.print(",");
			//Serial.print(index);
			//Serial.println("--");
			if(s[i]==',' || s[i]==';')
			{
				uint32_t iArg = s.substring(index,i).toInt();
				//Serial.print("Arg:");
				//Serial.println(iArg);
				Arg[Argnum++] =iArg;
				index = (i+1);
				if(pMessage[i]==';')
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CQuadcopterVisual::FecthArgsFloat(char *pMessage,uint8_t &iReadSize, float * Arg,uint8_t &Argnum)
{
	//Serial.print("FecthArgs:");
	//Serial.println(*pMessage);

	if(*pMessage==':')
	{
		uint8_t index=0;
		String s = ++pMessage;
		Argnum = 0;

		//Serial.print("iReadSize:");
		//Serial.println(iReadSize);
		for(uint8_t i=0; i<iReadSize;i++)
		{
			//Serial.print("--LOOP:");
			//Serial.print(i);
			//Serial.print(",");
			//Serial.print(index);
			//Serial.println("--");
			if(s[i]==',' || s[i]==';')
			{
				float iArg = s.substring(index,i).toFloat();
				//Serial.print("Arg:");
				//Serial.println(iArg);
				Arg[Argnum++] =iArg;
				index = (i+1);
				if(pMessage[i]==';')
				{
					return true;
				}
			}
		}
	}
	return false;
}


bool CQuadcopterVisual::Validate(char * &pMessage,uint8_t &iReadSize,const char * pValType)
{
	String sValType = pValType;
	uint8_t iValSize = sValType.length();

	if(iReadSize<iValSize)
	{
		return 0;
	}

	 String s = pMessage;
	 String subs = s.substring(0,iValSize);

	 bool res = !subs.compareTo(sValType);



	 //Serial.println("Validation:");
	 //Serial.print(subs);
	 //Serial.print(",");
	 //Serial.println(pValType);
	 //Serial.println("Return:");
	 //Serial.println(res);
	 //Serial.println("Size of :");
	 //Serial.println(iValSize);
	 //Serial.println(iReadSize);

	 if(res && iReadSize>iValSize)
	 {
		 //Serial.println(*pMessage);
		 pMessage+=iValSize;
		 //Serial.println(*pMessage);
		 iReadSize-=iValSize;
	 }

	 return res;

}




