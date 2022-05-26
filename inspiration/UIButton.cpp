/*
 * UIButton.cpp
 *
 *  Created on: 20/05/2015
 *      Author: Nilasstohr
 */

#include "UIButton.h"
#include "core_pins.h"
#include "serial_com.h"
#include "SyncEvent.h"

#define UIBUTTON_TEST

static CSyncEvent g_EventButton;
extern "C" void Irq(void){
	#ifdef UIBUTTON_TEST
		g_EventButton();
	#endif
}

CUIButton::CUIButton(uint8_t iDigitalPin):
m_iDigitalPin(iDigitalPin)
{
	Initialize();
	m_bIsButtonCliked = false;
	g_EventButton.Bind(this,&CUIButton::IrqButtonHandler);
}

CUIButton::~CUIButton()
{
	// TODO Auto-generated destructor stub
}

void CUIButton::Initialize()
{
	pinMode(m_iDigitalPin, INPUT);

	digitalWrite(m_iDigitalPin, LOW);

	attachInterrupt(m_iDigitalPin,Irq,CHANGE);

	#ifdef UIBUTTON_TEST
		Serial.begin(9600);
	#endif
}

void CUIButton::IrqButtonHandler()
{
	Serial.print("Pushed\r\n");
}

bool CUIButton::ClickListener()
{
	if(m_bIsButtonCliked){
		return true;
	}
	return false;
}
