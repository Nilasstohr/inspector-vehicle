/*
 * UIButton.h
 *
 *  Created on: 20/05/2015
 *      Author: Nilasstohr
 */

#ifndef UIBUTTON_H_
#define UIBUTTON_H_

#include <stdint.h>

class CUIButton {

	public:
		CUIButton(uint8_t iDigitalPin);

		virtual ~CUIButton();

		bool ClickListener();

	private:

		void Initialize();

		void IrqButtonHandler();

		uint8_t m_iDigitalPin;

		bool m_bIsButtonCliked;
};

#endif /* UIBUTTON_H_ */
