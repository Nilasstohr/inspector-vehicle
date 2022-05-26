//#define DEBUG
/*
 * BlueToothInterface.cpp
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */



#include "BlueToothInterface.h"
#include "serial_com.h"
#include "WCharacter.h"
#include "TeensyPins.h"

#define BAUDRATE_DEFAULT 115200
#define BT_DELAY_MS 10
String m_inString = "";
String m_GSPOutString = "";
String m_GSPInString = "";

CBlueToothInterface::CBlueToothInterface()
{
	m_bOpen = false;
	m_iBaudRate = BAUDRATE_DEFAULT;
}

CBlueToothInterface::~CBlueToothInterface()
{

}

bool CBlueToothInterface::Open(uint32_t iBaudRate, BTInterfaceMode Mode)
{
	m_iBaudRate = iBaudRate;

	switch(Mode)
	{
		case BT_ADMIND_MODE:
		{
			#ifdef DEBUG
			Serial.begin(m_iBaudRate);
			Serial1.begin(m_iBaudRate);
			char * conf= "$$$";
			return EnterConfigurationMode(conf);
			#endif
			return false;
		}
		case BT_NORMAL_MODE:
		{
			Serial1.begin(m_iBaudRate);
			Serial3.begin(9600);
			return true;
		}
		default:
		{
			return false;
		}
	}

	//Serial.begin(m_iBaudRate);
	return true;
}

bool CBlueToothInterface::EnterConfigurationMode(char *pConfCommand)
{
	bool bSuccessEnter = false;
	String inString = "";
	Serial1.print(pConfCommand);
	while(!bSuccessEnter)
	{
		if(Serial1.available())
		{
			 char inChar = Serial1.read();
			 inString += inChar;
			 if(inString=="CMD")
			 {
				 inString="";
				 bSuccessEnter=true;
			 }
		}
	}

	while(bSuccessEnter)
	{
		if(Serial1.available())  // If the bluetooth sent any characters
		{
		    // Send any characters the bluetooth prints to the serial monitor
			char BTByte= Serial1.read();
		  // Serial.print((char)Serial1.read());
		   Serial.print((char)BTByte);

		   inString+=BTByte;
		   if(inString=="END")
		   {
			   return true;
		   }

		}

		if(Serial.available())  // If stuff was typed in the serial monitor
		{
		    // Send any characters the Serial monitor prints to the bluetooth

		   char Byte = Serial.read();
		   Serial1.print(Byte);
		   inString="";
		}
	}

	return bSuccessEnter;
}

void CBlueToothInterface::Read(char * pMessage, uint8_t &iReadSize)
{
	uint8_t Size=0;
	bool bRead=true;
	while(bRead)
	{
		while(Serial1.available())
		{
			pMessage[Size] = Serial1.read();
			//Serial1.print(pMessage[Size]);
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
	uint8_t Size=0;
	while(Serial1.available())
	{
		pMessage[Size] = Serial1.read();
		Size++;
	}
	iReadSize = Size;
	*/
}

void CBlueToothInterface::ReadFast(char * pMessage, uint8_t &iReadSize)
{
	uint8_t Size=0;

	while(Serial1.available())
	{
		pMessage[Size] = Serial1.read();
		Size++;
	}
	iReadSize = Size;
}

void CBlueToothInterface::EmergencyStop()
{
	// be
}


void CBlueToothInterface::Write(const char * pMessage, uint8_t iWriteSize)
{
	for(uint8_t i=0; i<iWriteSize; i++)
	{
		Serial1.print(pMessage[i]);
	}
	//delay(BT_DELAY_MS);
}

void CBlueToothInterface::Echo()
{
	char aMessage[100];
	uint8_t iReadSize=0;
	Read(aMessage,iReadSize);
	Write(aMessage,iReadSize);
}

bool CBlueToothInterface::ToInt(long int &iValue){
	char inChar;
	while (Serial1.available()) {
		inChar = (char)Serial1.read();
		if(inChar=='d')
		{
			Serial1.println();
			iValue = m_inString.toInt();
			Serial1.print("Set: ");
		   	Serial1.println(iValue);
		   	m_inString = "";
		    return true;
		}
		else
		{
			Serial1.print(inChar);
			m_inString +=inChar;
			return false;
		}
	}
	return false;
}

bool CBlueToothInterface::WriteGSP()
{
	char inChar;
	while (Serial1.available()) {
		inChar = (char)Serial1.read();
		if(inChar=='d')
		{
			Serial1.println();
			Serial3.print("$GPRMC");
			Serial3.print("\r\n");
			Serial1.print("GPS Write: ");
		   	Serial1.println(m_GSPOutString);
		   	m_GSPOutString = "";
		    return true;
		}
		else
		{
			Serial1.print(inChar);
			m_GSPOutString +=inChar;
			return false;
		}
	}
	return false;
}

void CBlueToothInterface::ReadGSP()
{
	char inChar;
	while (Serial3.available())
	{
		inChar = (char)Serial3.read();
		m_GSPInString +=inChar;
	}
	Serial1.println();
	Serial1.print("GPS Read: ");
    Serial1.println(m_GSPInString);
	m_GSPInString = "";
}

void CBlueToothInterface::WriteFeedback(float pitch, float roll, float heading)
{
	 Serial1.print(" pitch: ");
	 Serial1.print(pitch, 2);
	 Serial1.print(" roll: ");
	 Serial1.print(roll, 2);
	 Serial1.print(" heading: ");
	 Serial1.println(heading, 2);
}

