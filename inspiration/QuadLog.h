/*
 * QuadLog.h
 *
 *  Created on: 11/10/2015
 *      Author: Nilasstohr
 */

#ifndef QUADLOG_H_
#define QUADLOG_H_

#include <stdlib.h>
#include "core_pins.h"


class CQuadLog {

	private:

		typedef struct _SLog
		{
			float fPicth;
			float fRoll;
			float Picth;
			float Roll;
			float Yaw;
		}SLog;

		typedef struct _SLogRaw
		{
			int16_t xg;
			int16_t yg;
			int16_t zg;
			int16_t xa;
			int16_t ya;
			int16_t za;
		}SLogRaw;

		typedef struct _SLogControl
		{
			float flex1;
			float flex2;
			float flex3;
			float flex4;
			float flex5;
			float flex6;
			float flex7;
			float flex8;
			float flex9;
			float flex10;
			float flex11;
			float flex12;
		}SLogControl;

	public:

		CQuadLog(uint32_t iCapacity,uint32_t iCapacityRaw,uint32_t iCapacityControl);
		virtual ~CQuadLog();

		bool     Open();


		uint32_t GetCapacity(){return  m_iCapacity;}
		void     SetItemCount(uint32_t iCount);
		uint32_t GetItemCount(){return m_iItemCount;}
		bool     GetItem(char *pItem, uint32_t &ItemSize);
		bool     SetItem(float fpitch, float froll, float pitch, float roll, float heading);

		SLog    *m_pLog;

		void     SetItemCountRaw(uint32_t iCount);
		uint32_t GetItemCountRaw(){return m_iItemCountRaw;}
		bool     GetItemRaw(char *pItem, uint32_t &ItemSize);
		bool     SetItemRaw(
					int16_t xg,
					int16_t yg,
					int16_t zg,
					int16_t xa,
					int16_t ya,
					int16_t za);

		SLogRaw    *m_pLogRaw;


		void     SetItemCountControl(uint32_t iCount);
		uint32_t GetItemCountControl(){return m_iItemCountControl;}
		bool GetItemControl(char *pItem, uint32_t &ItemSize);

		bool SetControlItem
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
		);

		SLogControl * m_pLogControl;


	private:

		void Init();

		uint32_t m_iCapacity;
		uint32_t m_iItemCount;
		uint32_t m_i;

		uint32_t m_iCapacityRaw;
		uint32_t m_iItemCountRaw;
		uint32_t m_iRaw;

		uint32_t m_iCapacityControl;
		uint32_t m_iItemCountControl;
		uint32_t m_iControl;

};

#endif /* QUADLOG_H_ */
