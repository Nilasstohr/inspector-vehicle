/*
 * QuadcopterWorker.cpp
 *
 *  Created on: 17/05/2015
 *      Author: Nilasstohr
 */

#include "QuadcopterWorker.h"
#include "core_pins.h"
#include <stdlib.h>
#include "RN41.h"

#define LED 13
#define DELAY_MS 800
#define BT_BAUD_RATE 115200

#define BT_REMOTE_CONTROLLER_ADRESS "000666698CDB"

#define MS1_PWM 13   //13
#define MS2_PWM 25 //25
#define MS1_2_PWM 20

//#define WORKER_DEBUG

CQuadcopterWorker::CQuadcopterWorker(
	CQuadcopterController *pQuadcopterController,
	CQuadcopterVisual     *pQuadCopterVisual
):
m_pQuadcopterController(pQuadcopterController),
m_pQuadCopterVisual(pQuadCopterVisual),
m_eWorker(WORKER_CONTROLLER),
//m_eWorker(WORKER_COM),
//m_eInterface(CQuadcopterVisual::INTERFACE_BT)
m_eInterface(CQuadcopterVisual::INTERFACE_WIRE)
{
	m_bOpen=false;
}

CQuadcopterWorker::~CQuadcopterWorker()
{

}

void CQuadcopterWorker::Open()
{
	m_pQuadCopterVisual->Open();
	m_pQuadcopterController->Open();
	m_bOpen = true;
	Run();
}

void CQuadcopterWorker::Run()
{
	//delay(10000);
	//Serial1.print("Worker entry\r\n");

	while(m_bOpen)
	{
		switch(m_eWorker)
		{
		    case WORKER_COM:		   WorkerCom();            break;
		    case WORKER_CONTROLLER:	   WorkerController();     break;
			case WORKER_SUB_CLASSES:   WorkerSubClasses();	   break;

		}
	}
}

void CQuadcopterWorker::WorkerCom()
{
	uint8_t iCase=0;
	if(m_pQuadCopterVisual->Com(iCase,m_eInterface))
	{
		SetCom(iCase);
	}
}

void CQuadcopterWorker::WorkerController()
{
	m_pQuadcopterController->Run();
	SetCom(0);
}

void CQuadcopterWorker::SetCom(uint8_t iCase)
{
	switch(iCase)
	{
		case 0: m_eWorker=WORKER_COM; break;
		case 1: m_eWorker=WORKER_CONTROLLER; break;
		case 2: m_eWorker=WORKER_SUB_CLASSES; break;
		default:
			break;
	}
}

void CQuadcopterWorker::WorkerSubClasses()
{
	while(1)
	{
		Serial.print("hello");
		delay(1000);
	}
}

void CQuadcopterWorker::Calibration()
{
	/*
	long int ivalue=0;

	if(m_pCBlueToothInterface->ToInt(ivalue))
	{
		m_pQuadcopterController->UpdateSpeed((uint8_t)ivalue,(uint8_t)ivalue,(uint8_t)ivalue,(uint8_t)ivalue);
	}
	*/
}

void CQuadcopterWorker::GPS()
{
	//if(m_pCBlueToothInterface->WriteGSP())
	//{
		//m_pCBlueToothInterface->ReadGSP();
	//}
}

void CQuadcopterWorker::GetFeedBack()
{
	/*
	float pitch, roll, heading;
	if(m_pQuadcopterController->GetFeedback(pitch, roll, heading))
	{
	     Serial.print(pitch, 2);
	     Serial.print(" ");
		 Serial.print(roll, 2);
		 Serial.print(" ");
		 Serial.println(heading, 2);
	}
	else
	{
		Serial.println("Failed to read");
	}
	delay(200);
	*/
}

void CQuadcopterWorker::BlinkWorker(){
	digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
	delay(DELAY_MS);               // wait for a second
	digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
	delay(DELAY_MS);               // wait for a second
}
