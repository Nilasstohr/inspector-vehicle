/*
 * QuadLog.cpp
 *
 *  Created on: 11/10/2015
 *      Author: Nilasstohr
 */

#include "QuadLog.h"
#include <stdio.h>
#include "serial_com.h"
#include "String.h"


CQuadLog::CQuadLog(uint32_t iCapacity,uint32_t iCapacityRaw, uint32_t iCapacityControl):
m_iCapacity(iCapacity),
m_iCapacityRaw(iCapacityRaw),
m_iCapacityControl(iCapacityControl),
m_iItemCount(0),
m_i(0),
m_iItemCountRaw(0),
m_iRaw(0),
m_iItemCountControl(0),
m_iControl(0)

{
	m_pLog = new _SLog[m_iCapacity];
	m_pLogRaw = new _SLogRaw[m_iCapacityRaw];
	m_pLogControl = new _SLogControl[m_iCapacityControl];
}

CQuadLog::~CQuadLog()
{

}

bool CQuadLog::Open()
{
	Init();
	return true;
}

void CQuadLog::Init()
{
	for(uint8_t i=0; i<m_iCapacity; i++)
	{
		m_pLog[i].fPicth=0;
		m_pLog[i].fRoll=0;
		m_pLog[i].Picth=0;
		m_pLog[i].Roll=0;
		m_pLog[i].Yaw=0;
	}

	for(uint8_t i=0; i<m_iCapacityRaw; i++)
	{
		m_pLogRaw[i].xg=0;
		m_pLogRaw[i].yg=0;
		m_pLogRaw[i].zg=0;
		m_pLogRaw[i].xa=0;
		m_pLogRaw[i].ya=0;
		m_pLogRaw[i].za=0;
	}

	for(uint8_t i=0; i<m_iCapacityControl; i++)
	{
		m_pLogControl[m_iControl].flex1 = 0;
		m_pLogControl[m_iControl].flex2 = 0;
		m_pLogControl[m_iControl].flex3 = 0;
		m_pLogControl[m_iControl].flex4 = 0;
		m_pLogControl[m_iControl].flex5 = 0;
		m_pLogControl[m_iControl].flex6 = 0;
		m_pLogControl[m_iControl].flex7 = 0;
		m_pLogControl[m_iControl].flex8 = 0;
		m_pLogControl[m_iControl].flex9 = 0;
		m_pLogControl[m_iControl].flex10 = 0;
		m_pLogControl[m_iControl].flex11 = 0;

	}
}

void CQuadLog::SetItemCount(uint32_t iCount)
{
	if(iCount > m_iCapacity)
	{
		m_iItemCount = m_iCapacity;
		return;
	}
	m_iItemCount = iCount;
}

bool CQuadLog::GetItem(char *pItem, uint32_t &ItemSize)
{
	ItemSize = sprintf(pItem,"LOGS:%f,%f,%f,%f,%f;",
			m_pLog[m_i].fPicth,
			m_pLog[m_i].fRoll,
			m_pLog[m_i].Picth,
			m_pLog[m_i].Roll,
			m_pLog[m_i].Yaw
     );
	//ItemSize = sizeof(pItem);

	if(m_iItemCount>=m_i)
	{
		m_i++;
		return true;
	}
	else{
		m_i=0;
		return false;
	}
}

bool CQuadLog::SetItem(float fpitch, float froll, float pitch, float roll, float heading)
{
	m_pLog[m_i].fPicth = fpitch;
    m_pLog[m_i].fRoll  = froll;
    m_pLog[m_i].Picth  = pitch;
    m_pLog[m_i].Roll   = roll;
	m_pLog[m_i].Yaw   = heading;

	if(m_iItemCount>=m_i)
	{
		m_i++;
		return true;
	}
	else
	{
		m_i=0;
		return false;
	}
}

//*******************************************************************************
//									Control
//*******************************************************************************

bool CQuadLog::SetControlItem
(
	float flex1,
	float flex2,
	float flex3,
	float flex4,
	float flex5,
	float flex6,
	float flex7,
	float flex8,
	float flex9,
	float flex10,
	float flex11,
	float flex12
)
{

	m_pLogControl[m_iControl].flex1  = flex1;
	m_pLogControl[m_iControl].flex2  = flex2;
	m_pLogControl[m_iControl].flex3  = flex3;
	m_pLogControl[m_iControl].flex4  = flex4;
	m_pLogControl[m_iControl].flex5  = flex5;
	m_pLogControl[m_iControl].flex6  = flex6;
	m_pLogControl[m_iControl].flex7  = flex7;
	m_pLogControl[m_iControl].flex8  = flex8;
	m_pLogControl[m_iControl].flex9  = flex9;
	m_pLogControl[m_iControl].flex10 = flex10;
	m_pLogControl[m_iControl].flex11 = flex11;
	m_pLogControl[m_iControl].flex12 = flex12;

	/*
	Serial.print("Set m_iControl=");
	Serial.print(m_iControl);
	Serial.print(" m_iItemCountControl=");
	Serial.println(m_iItemCountControl);
	*/

	/*
	Serial.print("SetControlItem: ");
	Serial.print(m_pLogControl[m_iControl].Rollmes);
	Serial.print(" ");
	Serial.print(m_pLogControl[m_iControl].Pitchmes);
	Serial.print(" ");
	Serial.println(m_pLogControl[m_iControl].URoll);
	*/

	//delay(1000);

	if(m_iItemCountControl>=m_iControl)
	{
		m_iControl++;
		return true;
	}
	else
	{
		m_iControl=0;
		return false;
	}
}

void CQuadLog::SetItemCountControl(uint32_t iCount)
{
	if(iCount > m_iCapacityControl)
	{
		m_iItemCountControl = m_iCapacityControl;
		return;
	}
	m_iItemCountControl = iCount;
}

bool CQuadLog::GetItemControl(char *pItem, uint32_t &ItemSize)
{
	//ItemSize = sprintf(pItem,"LOGS:%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f;\r\n",
	ItemSize = sprintf(pItem,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\r\n",

		m_pLogControl[m_iControl].flex1,
		m_pLogControl[m_iControl].flex2,
		m_pLogControl[m_iControl].flex3,
		m_pLogControl[m_iControl].flex4,
		m_pLogControl[m_iControl].flex5,
	    m_pLogControl[m_iControl].flex6,
		m_pLogControl[m_iControl].flex7,
		m_pLogControl[m_iControl].flex8,
		m_pLogControl[m_iControl].flex9,
		m_pLogControl[m_iControl].flex10,
		m_pLogControl[m_iControl].flex11,
		m_pLogControl[m_iControl].flex12
	);


	/*
	Serial.print("Get m_iControl=");
	Serial.print(m_iControl);
	Serial.print(" m_iItemCountControl=");
	Serial.println(m_iItemCountControl);
	 */

	//ItemSize = sizeof(pItem);

	if(m_iItemCountControl>=m_iControl)
	{
		m_iControl++;
		return true;
	}
	else
	{
		m_iControl=0;
		return false;
	}
}

//*******************************************************************************
//									Raw
//*******************************************************************************

void CQuadLog::SetItemCountRaw(uint32_t iCount)
{
	if(iCount > m_iCapacityRaw)
	{
		m_iItemCount = m_iCapacityRaw;
		return;
	}
	m_iItemCountRaw = iCount;
}

bool CQuadLog::SetItemRaw(
	int16_t xg,
	int16_t yg,
	int16_t zg,
	int16_t xa,
	int16_t ya,
	int16_t za
)
{
	m_pLogRaw[m_iRaw].xg = xg;
	m_pLogRaw[m_iRaw].yg = yg;
	m_pLogRaw[m_iRaw].zg = zg;

	m_pLogRaw[m_iRaw].xa = xa;
	m_pLogRaw[m_iRaw].ya = ya;
	m_pLogRaw[m_iRaw].za = za;


	if(m_iItemCountRaw>=m_iRaw)
	{
		m_iRaw++;
		return true;
	}
	else
	{
		m_iRaw=0;
		return false;
	}
}

bool CQuadLog::GetItemRaw(char *pItem, uint32_t &ItemSize)
{
	ItemSize = sprintf(pItem,"LOGS:%d,%d,%d,%d,%d,%d",
		m_pLogRaw[m_iRaw].xg,
		m_pLogRaw[m_iRaw].yg,
		m_pLogRaw[m_iRaw].zg,
		m_pLogRaw[m_iRaw].xa,
		m_pLogRaw[m_iRaw].ya,
		m_pLogRaw[m_iRaw].za
	);
	//ItemSize = sizeof(pItem);

	if(m_iItemCountRaw>=m_iRaw)
	{
		m_iRaw++;
		return true;
	}
	else
	{
		m_iRaw=0;
		return false;
	}
}


