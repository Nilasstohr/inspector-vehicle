/*
 * QuadcopterController.h
 *
 *  Created on: 30/05/2015
 *      Author: Nilasstohr
 */

#ifndef QUADCOPTERCONTROLLER_H_
#define QUADCOPTERCONTROLLER_H_

#include "core_pins.h"
#include <stdlib.h>
#include "IntervalTimer.h"
#include "QuadCopterFeedBack.h"
#include "QuadcopterVisual.h"
#include "QuadLog.h"
#include "QuadSpeedControllerDriver.h"
#include "QuadJoyStickDriver.h"
#include "LIDARLite.h"

class CQuadcopterController {

	public:

			enum EController
			{
				CONTROLLER_COM = 0,
				CONTROLLER_END,
				CONTROLLER_SPEEDSET,
				CONTROLLER_LOG,
				CONTROLLER_PRINT,
				CONTROLLER_CONTROL,
				CONTROLLER_TEST_LOGTIMER,
				CONTROLLER_REMOTE_JOYSTICK,
				CONTROLLER_TEST_ATTITUDE_SENSOR
			};

			enum ELog
			{
				LOG_COM = 0,
				LOG_END,
				LOG_SETTINGS,
				LOG_START,
				LOG_SETTINGS_RAW,
				LOG_START_RAW
			};

			enum EControl
			{
				CONTROL_COM = 0,
				CONTROL_END,
				CONTROL_SETTINGS,
				CONTROL_START,
				CONTROL_TRANSMIT,
				CONTROLLER_CALC_ANGLES,
				CONTROLLER_TRANSMIT_ANGLES
			};

			enum EControlLogState
			{
				LOG_CONTROL_NON=0,
				LOG_CONTROL_REAL_TIME,
				LOG_CONTROL_RECORD,
				LOG_CONTROL_PRINT,
				LOG_CONTROL_CAL
			};

			enum EControlLogType
			{
				TYPE_CONTROL_PITCH_ROLL_ALTITUDE=0,
				TYPE_CONTROL_RAW_MEMS,
				TYPE_CONTROL_LOG_INTERNAL,
				TYPE_CONTROL_FORCES
			};


			// CONSET:2,3,1000000,10,1,0,5;


		CQuadcopterController(
			CQuadCopterFeedBack	  *pQuadCopterFeedBack,
			CQuadcopterVisual     *pQuadcopterVisual,
			CQuadLog 			  *pQuadLog,
			CQuadSpeedControllerDriver    *pQuadSpeedControl,
			CQuadJoyStickDriver *pQuadJoyStic,
			LIDARLite          * pLIDARLite,
			float iTs
		);

		virtual ~CQuadcopterController();

		bool GetFeedback(
			float &fpitch,
			float &froll,
			float &pitch,
			float &roll,
			float &heading
		);

		bool GetFeedbackNoFilter(
			float &pitch,
			float &roll);

		bool GetFeedbackCal(
			float &fpitch,
			float &froll,
			float &pitch,
			float &roll,
			float &heading
		);

		bool GetFeedbackRaw(
			int16_t &xg,
			int16_t &yg,
			int16_t &zg,
			int16_t &xa,
			int16_t &ya,
			int16_t &za
		);

		bool GetConvertFactor(float &gRes, float &aRes);
		bool GetBias(float* gBias, float * aBias);
		bool SetBias();
		bool Open();

		void Run();


	private:

		void ControllerCom();
		bool Initialize();
		void SetCom(uint8_t iCase);
		void ReadReset();
		void Ack();
		void EmergencyStop();

		// Logging
		void FeedbackLog();
		void LogCom();
		void LogSettings();
		void LogSettingsRaw();
		void Log();
		void ResetLog();
		void LogRaw();
		bool FetchLogArg(char *pMessage,uint8_t iReadSize,uint32_t &iSampleRate,uint32_t &iLogSize);
		bool SetLogCom(uint8_t iCase);

		// Control
		void ControllerTransmitAngles();
		void ControllerTransmitForces();
		void ControllerTransmitInternalControlValues();
		void ControllerTransmitMemsRaw();
		void ControllerCalcAngles();
		void ControllerCalcForce();
		void Controller();
		void InitPitchController();
		void IrqSampleTimerHandler();
		void IrqLogTimerHandler();
		void InitSampleTimer();
		void InitLogTimer();
		void EndLogTimer();
		void PrintFeedback();
		void ControlCom();
		bool SetControlCom(uint8_t iCase);
		void Control();
		void ControlSettings();
		void SetReferences(float Pitch, float Roll, float Altitude);
		bool SetControlSitting(char * pMessage, uint8_t iReadSize);
		bool InterpretComControl(char * pMessage, uint8_t iReadSize);
		void SetHoowerStep(uint8_t iCHooverWeight,uint8_t iDirection);
		void SetHoower(uint16_t iCHooverWeight);
		void HoowerSaturation(float &UHoower);
		void HoowerSaturation2(float &UHoower);
		bool SetControlParam(float* pArg);
		void ControllerTransmit();
		void ResetLogControl();
		void SpeedSet();
		void TestRemoteJoyStick();
		void TestAttitudeSensor();

		typedef struct _SControlParam
		{
			float Pref;
			float Pmesf;
			float Rmesf;
			float Pmes;
			float Rmes;
			float Ymes;
			float P;
			float e;
			float TH;
			float u;

		}SConP;

		SConP *m_pConP;

		typedef struct _SLogParam
		{
		    uint32_t iSampleRate;
		    uint32_t iLogNum;

		}SLogParam;

		void SetLogParam(uint32_t SampleRate,uint32_t LogNum);

		SLogParam *m_pLogParam;


		typedef struct PIDController
		{
			float Kp;
			float Kd;
			float Ki;
			float uILast;
			float udLast;
			float up=0;
			float ud=0;
			float ui=0;
			float uiTres;
			float e;
			float ref;
			bool State;
		}CPid;


		typedef struct ControllerSat
		{
			uint16_t SatHigh;
			uint16_t SatLow;
			uint16_t SatHighHoow;
			uint16_t SatLowHoow;
		}CSat;

		CPid *m_CRoll;
		CPid *m_CPitch;
		CPid *m_CHoower;
		CSat *m_CSat;
		float m_Hoower;


		float m_Ts;

		typedef struct ControlLogParam
		{
			uint16_t Ts;
			uint32_t LogNum;
			uint32_t LogCount;
			uint8_t  LogState;
			uint8_t  LogType;
		}CLogParam;

		CLogParam *m_CLogParam;

		float m_GyroRes;
		float m_AccRes;
		float m_GyroGain;
		float m_AccGain;
		float m_CalcMode;

		void InitRollPIDController();
		void InitPitchPIDController();
		void InitControllerSaturation();
		void InitHoowerPIDController();
		void InitControlLog();
		void RollPIDController(float MRoll, float &URoll);
		void PitchPIDController(float MPitch, float &UPitch);
		void HoowerPIDController(float MHoower, float &UHoower);
		void XConfigurationMixer
		(
			float URoll,
			float UPitch,
			float CHooverForce,
			float &M1,
			float &M2,
			float &M3,
			float &M4
		);

		void XConfigurationMixer2
		(
			float URoll,
			float UPitch,
			float CHooverForce,
			float &M1,
			float &M2,
			float &M3,
			float &M4
		);


		float Thrust2DutyMS(float Thrust);
		float Thrust2PWM(float Thrust);
		float Thrust2PWM2(float Thrust);
		void TestDisSensor();

		float m_CHooverWeight;

		bool FetchSpeed(
			char * pMessage,
			uint8_t iReadSize,
			uint16_t &iSpeedMCounterW_1,
			uint16_t &iSpeedMCounterW_2,
			uint16_t &iSpeedMClockW_1,
			uint16_t &iSpeedMClockW_2
		);

		bool SetControlLogState(uint8_t State);
		EControlLogState GetControlLogState();

		bool SetControlLogType(uint8_t State);
		EControlLogType GetControlLogType();

		bool m_bInterLoggerDone;
		void TestLogTimer();

		typedef struct ControlLogVaribles
		{
			float f1;
			float f2;
			float f3;
			float f4;
			float f5;
			float f6;
			float f7;
			float f8;
			float f9;
			float f10;
			float f11;
			float f12;
		}CLogVariable;

		CLogVariable *m_LogVars;

		// for logging
		float m_lxg;
		float m_lyg;
		float m_lzg;
		float m_lxa;
		float m_lya;
		float m_lza;

		float m_Rollmes;
		float m_Pitchmes;
		float m_Headmes;
		float m_RollmesNoF;
		float m_PitchmesNoF;


		float m_Hoowermes;

		void LogHandle();

		bool m_bOpen;

		CQuadCopterFeedBack	  *m_pQuadCopterFeedBack;
		CQuadcopterVisual     *m_pQuadcopterVisual;
		CQuadLog 			  *m_pQuadLog;

		EController m_eController;
		ELog m_eLog;
		volatile EControl m_eControl;
		EControlLogState  m_eControlLogState;
		EControlLogType  m_eControlLogType;

		//Com
		char m_aMessage[300];
		uint8_t m_iReadSize =0;

		CQuadcopterVisual::EInterFace m_eInterface;
		IntervalTimer * m_pSampleTimer;
		IntervalTimer * m_pLogTimer;

		uint32_t m_SampleTimerTicks;
		uint32_t m_LogControllerInterval;
		bool m_ControlMixer2;
		bool m_MotorState;
		bool m_FlightControllerMode;

		bool  m_bAutoThrust;
		float m_AutoThrustCount;
		float m_AutoThrustTimeOut;
		float m_StepPWMVaule;
		uint16_t m_PWM;

		float m_AutoThrust[11];
		uint8_t m_AutoThrustc;


		uint16_t m_Thrustmes;
		uint16_t m_ThrustmesOffSet;
		void TransmitParam(uint16_t ThrustmesOffSet);

		int32_t m_PosCor_Thrust=0;

		CQuadSpeedControllerDriver    *m_pQuadSpeedControl;
		CQuadJoyStickDriver *m_pQuadJoyStick;
		LIDARLite *m_pLIDARLite;

};

#endif /* QUADCOPTERCONTROLLER_H_ */
