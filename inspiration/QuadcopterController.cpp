/*
 * QuadcopterController.cpp
 *
 *  Created on: 30/05/2015
 *      Author: Nilasstohr
 */

#include "QuadcopterController.h"
#include "SyncEvent.h"
#include <stdio.h>

//#define CONTROLLER_DEBUG


#define THRUST 3800
#define PWM_MIN 19669
#define PWM_MAX 39321

 uint32_t gospeed = 21500;
 bool go = true;


int TestInt=0;

volatile bool g_SampleReady = false;

static CSyncEvent g_SampleEventTimer;
static CSyncEvent g_LogEventTimer;

extern "C" void IrqSampleTimer(void){
	g_SampleEventTimer();
}

extern "C" void IrqLogTimer(void){
	g_LogEventTimer();
}

CQuadcopterController::CQuadcopterController(
	CQuadCopterFeedBack	  *pQuadCopterFeedBack,
	CQuadcopterVisual     *pQuadcopterVisual,
	CQuadLog 			  *pQuadLog,
	CQuadSpeedControllerDriver    *pQuadSpeedControl,
	CQuadJoyStickDriver *pQuadJoyStick,
	LIDARLite          * pLIDARLite,
	float iTs
):
m_pQuadCopterFeedBack(pQuadCopterFeedBack),
m_pQuadcopterVisual(pQuadcopterVisual),
m_pQuadLog(pQuadLog),
m_pQuadSpeedControl(pQuadSpeedControl),
m_pQuadJoyStick(pQuadJoyStick),
m_pLIDARLite(pLIDARLite),
//m_eController(CONTROLLER_TEST_DISSENSOR),
//m_eController(CONTROLLER_TEST_RPMSENSOR),
m_eController(CONTROLLER_TEST_ATTITUDE_SENSOR),
m_eLog(LOG_COM),
m_eControl(CONTROL_COM),
//m_eControl(CONTROL_START),
//m_eInterface(CQuadcopterVisual::INTERFACE_BT),
m_eInterface(CQuadcopterVisual::INTERFACE_WIRE),
m_Ts(iTs),
m_CHooverWeight(150),   // 275 gram
m_FlightControllerMode(false),
m_SampleTimerTicks(0),
m_LogControllerInterval(1),
m_bInterLoggerDone(false),
m_eControlLogState(LOG_CONTROL_NON),
m_MotorState(false),
m_eControlLogType(TYPE_CONTROL_PITCH_ROLL_ALTITUDE),
m_GyroGain(1),
m_AccGain(1),
m_bAutoThrust(false),
m_PWM(1)
{

	m_bOpen = false;
	m_pLogParam    = new _SLogParam;
	m_pConP		   = new _SControlParam;
	m_pSampleTimer = new IntervalTimer;
	m_pLogTimer    = new IntervalTimer;

	m_CRoll  = new PIDController;
	m_CPitch = new PIDController;
	m_CHoower = new PIDController;
	m_CSat   = new ControllerSat;
	m_CLogParam = new ControlLogParam;

	m_LogVars = new CLogVariable;

	InitPitchController();

	InitControllerSaturation();
	InitRollPIDController();
	InitPitchPIDController();
	InitHoowerPIDController();
	InitControlLog();

	m_Rollmes=0;
	m_Pitchmes=0;
	m_Hoowermes=0;

	g_SampleEventTimer.Bind(this,&CQuadcopterController::IrqSampleTimerHandler);
	g_LogEventTimer.Bind(this,&CQuadcopterController::IrqLogTimerHandler);

	//InitSampleTimer();

	SetControlLogState(0);
	SetLogParam(100,100);

}

CQuadcopterController::~CQuadcopterController()
{
	// TODO Auto-generated destructor stub
}

bool CQuadcopterController::Open()
{
	if(Initialize())
	{
		InitSampleTimer();
		m_bOpen = true;
	}
	return m_bOpen;
}

void CQuadcopterController::Run()
{

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Controller Run\r\n",18,m_eInterface);
#endif

	while(m_bOpen)
	{
		switch(m_eController)
		{
			case CONTROLLER_COM:        		 ControllerCom(); 	break;
		    case CONTROLLER_SPEEDSET:   		 SpeedSet();      	break;
		    case CONTROLLER_LOG:        		 FeedbackLog();   	break;
		    case CONTROLLER_PRINT:      		 PrintFeedback(); 	break;
		    case CONTROLLER_CONTROL:		     Control();    	    break;
		    case CONTROLLER_TEST_LOGTIMER:	     TestLogTimer();    break;
		    case CONTROLLER_REMOTE_JOYSTICK:     TestRemoteJoyStick(); break;
		    case CONTROLLER_TEST_ATTITUDE_SENSOR: TestAttitudeSensor(); break;

		    case CONTROLLER_END: 	                         		return;
		}
		delay(500);
	}
}

void CQuadcopterController::TestLogTimer()
{

	m_LogControllerInterval = 1000000;
	InitLogTimer();
	SetCom(CONTROLLER_COM);
}

void CQuadcopterController::ControllerCom()
{
	uint8_t iCase=0;

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Controller Com \r\n",17,m_eInterface);
#endif



	if(m_pQuadcopterVisual->Com(iCase,m_eInterface))
	{
		SetCom(iCase);
	}
}

void CQuadcopterController::SetCom(uint8_t iCase)
{
	switch(iCase)
	{
		case 0: m_eController=CONTROLLER_COM;        	 break;
		case 1: m_eController=CONTROLLER_END;        	 break;
		case 2: m_eController=CONTROLLER_SPEEDSET;  	 break;
		case 3: m_eController=CONTROLLER_LOG;        	 break;
		case 4: m_eController=CONTROLLER_PRINT;      	 break;
		case 5: m_eController=CONTROLLER_CONTROL;      	 break;
		case 6: m_eController=CONTROLLER_TEST_LOGTIMER;  break;
		case 7: m_eController=CONTROLLER_REMOTE_JOYSTICK;  break;

		default:
			break;
	}
}

bool CQuadcopterController::SetLogCom(uint8_t iCase)
{
	switch(iCase)
	{
		case 0: m_eLog=LOG_COM;			 return true;
		case 1: m_eLog=LOG_END; 		 return true;
		case 2: m_eLog=LOG_SETTINGS;     return true;
		case 3: m_eLog=LOG_START;        return true;
		case 4: m_eLog=LOG_SETTINGS_RAW; return true;
		case 5: m_eLog=LOG_START_RAW;    return true;
		default:
			return false;
	}
}

void CQuadcopterController::TestRemoteJoyStick()
{
	float U_Throtle;

	float MinVoltage = m_pQuadJoyStick->GetVoltageMax();
	float MaxVoltage = m_pQuadJoyStick->GetVoltageMin();
	float MaxBin     = m_pQuadSpeedControl->GetSpeedBinMax();
	float MinBin     = m_pQuadSpeedControl->GetSpeedBinMin();

	float a = (MinBin -MaxBin )/(MinVoltage - MaxVoltage) ;
	uint16_t b = MaxBin;
	uint16_t SCBin = MinVoltage;

	while(1)
	{
		U_Throtle = m_pQuadJoyStick->GetThrotleVoltageFilter();
		SCBin     = (uint16_t)(a*U_Throtle + b);
		m_pQuadSpeedControl->SetSpeedBin(SCBin,SCBin,SCBin,SCBin);

		//Serial.print("SCBin:");
		Serial.println(SCBin);

		/*
		Serial.print("MinVoltage:");
		Serial.println(MinVoltage,3);
		Serial.print("MaxVoltage:");
	    Serial.println(MaxVoltage,3);
	    Serial.print("MaxBin:");
	    Serial.println(MaxBin,3);
	    Serial.print("MinBin:");
		Serial.println(MinBin,3);
		Serial.print("MinVoltage - MaxVoltage:");
		Serial.println(MinVoltage - MaxVoltage,3);
		Serial.print("MinBin -MaxBin:");
		Serial.println(MinBin -MaxBin,3);
		Serial.print("a:");
		Serial.println(a,3);
		Serial.print("SCBin:");
		Serial.println(SCBin);
		Serial.print("Out:");
		Serial.println(a*U_Throtle + b,3);
		*/

		delay(6);
	}
}

void CQuadcopterController::TestAttitudeSensor()
{
	int cal_cnt = 0;
	int dist;



	while(1){
		// take a measurement with receiver bias correction
	   if ( cal_cnt == 0 ) {
		 dist = m_pLIDARLite->distance();      // With bias correction
	   } else {
		 dist = m_pLIDARLite->distance(false); // Without bias correction
	   }

	   while(1)
	   		 Serial.println(dist);


	   // Increment reading counter
	   cal_cnt++;
	   cal_cnt = cal_cnt % 100;

	   // Display distance
	   Serial.println(dist);
	   //Serial.println(" cm");

	   delay(3);
	}

}

void CQuadcopterController::SpeedSet()
{
	//m_pQuadcopterVisual->Ack(m_eInterface);
	//SetCom(CONTROLLER_SPEEDSET);
	//delay(10);

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("SpeedSet entry \r\n",17,m_eInterface);
#endif


	m_iReadSize=0;
	m_pQuadcopterVisual->Read(m_aMessage,m_iReadSize,m_eInterface);

	if(m_iReadSize>0)
	{
		uint16_t iM1=0;
		uint16_t iM2=0;
		uint16_t iM3=0;
		uint16_t iM4=0;
		if(FetchSpeed(
			m_aMessage,
			m_iReadSize,
			iM1,
			iM2,
			iM3,
			iM4
		))
		{

			m_pQuadSpeedControl->SetSpeedBin(iM1,iM2, iM3,iM4);
			ReadReset();
			//Ack();
			SetCom(CONTROLLER_COM);
		}
	}
}

void CQuadcopterController::EmergencyStop()
{
	//UpdateSpeed(LOW_TRESHHOLD,LOW_TRESHHOLD, LOW_TRESHHOLD,LOW_TRESHHOLD);
}

void CQuadcopterController::Ack()
{
	m_pQuadcopterVisual->Ack(m_eInterface);
}

void CQuadcopterController::ReadReset()
{
	for(uint8_t i=0; i<sizeof(m_aMessage);i++)
	{
		m_aMessage[i] = '0';
	}
}

bool CQuadcopterController::Initialize()
{
	m_pQuadSpeedControl->init();
	m_pQuadJoyStick->init();
	m_pQuadCopterFeedBack->Open();
	m_pLIDARLite->begin(0,true);
	m_pLIDARLite->configure(0);

	return true;
}


bool CQuadcopterController::GetFeedback(
	float &fpitch,
	float &froll,
	float &pitch,
	float &roll,
	float &heading)
{
	if(m_pQuadCopterFeedBack->GetFeedbackComFilter(fpitch,froll,pitch,roll,heading,m_GyroGain,m_AccGain))
	{
		return true;
	}
	return false;
}

bool CQuadcopterController::GetFeedbackNoFilter(float &pitch, float &roll)
{
	float heading;
	if(m_pQuadCopterFeedBack->GetFeedback(pitch,roll,heading))
	{
		return true;
	}
	return false;
}

bool CQuadcopterController::GetFeedbackCal(
	float &fpitch,
	float &froll,
	float &pitch,
	float &roll,
	float &heading)
{
	if(m_pQuadCopterFeedBack->GetFeedbackComFilter(fpitch,froll,pitch,roll,heading,1,1))
	{
		return true;
	}
	return false;
}

bool CQuadcopterController::GetFeedbackRaw(
	int16_t &xg,
	int16_t &yg,
	int16_t &zg,
	int16_t &xa,
	int16_t &ya,
	int16_t &za
)
{
	if(m_pQuadCopterFeedBack->GetFeedbackRaw(xg,yg,zg,xa,ya,za))
	{
		return true;
	}
	return false;
}

bool CQuadcopterController::GetConvertFactor(float &gRes, float &aRes)
{
	if(m_pQuadCopterFeedBack->GetConvertFactor(gRes,aRes))
	{
		return true;
	}
	return false;
}

bool CQuadcopterController::SetBias()
{
	if(m_pQuadCopterFeedBack->SetBias())
	{
	   return true;
	}
    return false;
}

bool CQuadcopterController::GetBias(float *gBias, float *aBias)
{
   if(m_pQuadCopterFeedBack->GetBias(gBias,aBias))
   {
		return true;
   }
   return false;
}

void CQuadcopterController::FeedbackLog()
{

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Controller Log entry\r\n",22,m_eInterface);
#endif

	while(1)
	{
		switch(m_eLog)
		{
			case LOG_COM:            LogCom();         break;
			case LOG_SETTINGS:       LogSettings();    break;
		    case LOG_START:          Log();            break;
		    case LOG_SETTINGS_RAW:   LogSettingsRaw(); break;
		    case LOG_START_RAW:      LogRaw();         break;
			case LOG_END: 	     SetCom(CONTROLLER_COM); SetLogCom(LOG_COM); return;
		}
	}
}

void CQuadcopterController::LogCom()
{
	uint8_t iCase=0;
	if(m_pQuadcopterVisual->Com(iCase,m_eInterface))
	{
		SetLogCom(iCase);
	}
}

void CQuadcopterController::LogSettings()
{
	char pMessage[20];
	uint8_t iReadSize=0;

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Write Log Setting:\r\n",20,m_eInterface);
#endif


	m_pQuadcopterVisual->Read(pMessage,iReadSize,m_eInterface);
	if(iReadSize>0)
	{
		uint32_t iSampleRate=0,iLogSize=0;
		if(FetchLogArg(pMessage,iReadSize,iSampleRate,iLogSize))
		{
			SetLogParam(iSampleRate,iLogSize);
			SetLogCom(LOG_START);
		}
		else
		{
			Serial.print("FetchLogArg failed\r\n");
		}
	}
}

void CQuadcopterController::LogSettingsRaw()
{

	char pMessage[20];
	uint8_t iReadSize=0;

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Write Log Setting:\r\n",20,m_eInterface);
#endif


	m_pQuadcopterVisual->Read(pMessage,iReadSize,m_eInterface);
	if(iReadSize>0)
	{
		uint32_t iSampleRate=0,iLogSize=0;
		if(FetchLogArg(pMessage,iReadSize,iSampleRate,iLogSize))
		{
			SetLogParam(iSampleRate,iLogSize);
			SetLogCom(LOG_START_RAW);
		}
		else
		{
			Serial.print("FetchLogArg failed\r\n");
		}
	}
}

void CQuadcopterController::SetLogParam(uint32_t SampleRate,uint32_t LogNum)
{
	m_pLogParam->iLogNum = LogNum;
	m_pLogParam->iSampleRate = SampleRate;
}

bool CQuadcopterController::FetchLogArg(char *pMessage, uint8_t iReadSize, uint32_t &iSampleRate,uint32_t &iLogSize)
{
	uint32_t aArg[2];
	uint8_t iArg=0;
	if(m_pQuadcopterVisual->Validate(pMessage,iReadSize,"LOGS"))
	{
		if(m_pQuadcopterVisual->FecthArgs(pMessage,iReadSize,aArg,iArg))
		{
			if(iArg==2)
			{
				iSampleRate= aArg[0],
				iLogSize=	aArg[1];
				return true;
			}
		}
	}
	return false;
}

bool CQuadcopterController::FetchSpeed(
	char * pMessage,
	uint8_t iReadSize,
	uint16_t &iM1,
	uint16_t &iM2,
	uint16_t &iM3,
	uint16_t &iM4
)
{
	uint32_t aArg[4];
	uint8_t iArg=0;
	if(m_pQuadcopterVisual->Validate(pMessage,iReadSize,"SPEEDS"))
	{
		if(m_pQuadcopterVisual->FecthArgs(pMessage,iReadSize,aArg,iArg))
		{
			if(iArg==4)
			{
				iM1 = aArg[0];
			    iM2 = aArg[1];
				iM3 = aArg[2];
				iM4 = aArg[3];
				return true;
			}
		}
	}
	return false;
}

void CQuadcopterController::Log()
{
	m_pQuadLog->SetItemCount(m_pLogParam->iLogNum);
	bool m_bLog=true;
	float fpitch, froll, pitch, roll, heading;
	char  Item[150];
	uint32_t iReadSize;


#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Start Log\r\n",12,m_eInterface);
#endif


	while(m_bLog){
		if(GetFeedback(fpitch, froll,pitch,roll,heading))
		{
			if(!m_pQuadLog->SetItem(fpitch, froll,pitch,roll,heading))
			{

				#ifdef CONTROLLER_DEBUG
				m_pQuadcopterVisual->Write("-----------Log 1 Done----------\r\n",31,m_eInterface);
				#endif

				m_pQuadSpeedControl->SetSpeedControlleroff();
				ResetLog();

				Serial.print("LOGS:");
				Serial.print(m_pLogParam->iSampleRate);
				Serial.print(",");
				Serial.print(0);
				Serial.print(",");
				Serial.print(0);
				Serial.print(",");
				Serial.print(0);
				Serial.print(",");
				Serial.print(0);
				Serial.print(";");

				while(m_bLog)
				{
					if(m_pQuadLog->GetItem(Item,iReadSize))
					{
						delay(5);
						m_pQuadcopterVisual->Write(Item,iReadSize,m_eInterface);
					}
					else
					{
						delay(300);
						m_pQuadcopterVisual->Ack(m_eInterface);

						SetLogCom(LOG_END);
						//SetBias();
						return;
					}
				}
			}
			// Samplerate
			delayMicroseconds(m_pLogParam->iSampleRate);
		}
		else
		{
			Serial.write("Gyro not Open");
			return;
		}
	}
}

void CQuadcopterController::ResetLog()
{
	m_pQuadCopterFeedBack->ResetComFilter();
}

void CQuadcopterController::LogRaw()
{
	m_pQuadLog->SetItemCountRaw(m_pLogParam->iLogNum);
	bool m_bLog=true;

	int16_t xg=0;
	int16_t yg=0;
	int16_t zg=0;
	int16_t xa=0;
	int16_t ya=0;
	int16_t za=0;

	float gRes=0;
	float aRes=0;

	float gBias[3];
	float aBias[3];

	char  Item[200];
	uint32_t iReadSize=0;

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Start Log\r\n",12,m_eInterface);
#endif

	while(m_bLog)
	{
		if(GetFeedbackRaw(xg,yg,zg,xa,ya,za))
		{
			if(!m_pQuadLog->SetItemRaw(xg,yg,zg,xa,ya,za))
			{

				#ifdef CONTROLLER_DEBUG
				m_pQuadcopterVisual->Write("-----------Log 1 Done----------\r\n",31,m_eInterface);
				#endif

				GetConvertFactor(gRes,aRes);
				GetBias(gBias,aBias);

				Serial.print("LOGS:");
				Serial.print(m_pLogParam->iSampleRate);
				Serial.print(",");
				Serial.print(gRes,6);
				Serial.print(",");
				Serial.print(aRes,6);
				Serial.print(",");
				Serial.print(0);
				Serial.print(",");
				Serial.print(0);
				Serial.print(",");
				Serial.print(0);

				delay(10);

				Serial.print("LOGS:");
				Serial.print(gBias[0],6);
				Serial.print(",");
				Serial.print(gBias[1],6);
				Serial.print(",");
			    Serial.print(gBias[2],6);
			    Serial.print(",");
				Serial.print(aBias[0],6);
				Serial.print(",");
				Serial.print(aBias[1],6);
				Serial.print(",");
				Serial.print(aBias[2],6);

				delay(10);

				m_pQuadSpeedControl->SetSpeedControlleroff();

				while(m_bLog)
				{
					if(m_pQuadLog->GetItemRaw(Item,iReadSize))
					{
						delay(10);
						m_pQuadcopterVisual->Write(Item,iReadSize,m_eInterface);
					}
					else
					{
						delay(100);
						//m_pQuadcopterVisual->Ack(m_eInterface);
						SetBias();
						SetLogCom(LOG_END);
						return;
					}
				}
			}
			// Samplerate
			delayMicroseconds(m_pLogParam->iSampleRate);
		}
		else
		{
			Serial.write("Gyro not Open");
			return;
		}
	}
}

void CQuadcopterController::PrintFeedback()
{
	//Serial.begin(115200);

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Controller PrintFeedback\r\n",26,m_eInterface);
#endif

	while(1)
	{
		//Serial.println("PrintFeedback: ");

		if(g_SampleReady)
		{
			//Serial.print("Pitch: ");
			//Serial.println(m_Pitchmes);
			//Serial.print("Roll: ");
			Serial.print(m_Pitchmes);
			Serial.print(" ");
			Serial.println(m_Rollmes);

			//g_SampleReady = false;
		}
		delay(10);
	}

	/*
	while(go)
	{
//		UpdateSpeed(gospeed,gospeed,gospeed,gospeed);
		UpdateSpeed(Ms2Pwm(1),Ms2Pwm(1),Ms2Pwm(1),Ms2Pwm(1));

		Serial.println(Ms2Pwm(1.3));


		if(gospeed>35500)
		{
			go=false;
			gospeed=19500;
			UpdateSpeed(gospeed,gospeed,gospeed,gospeed);
		}
		else
		{
			//gospeed+=1000;
		}

		delay(1000);
	}
*/
}

void CQuadcopterController::InitPitchController()
{
	m_pConP->P = 5;
	m_pConP->Pref = 0;
	m_pConP->Pmes = 0;
	m_pConP->e =0;
	m_pConP->TH = 7;
}

void CQuadcopterController::IrqSampleTimerHandler()
{
	if(GetFeedback(m_Pitchmes,m_Rollmes,m_PitchmesNoF,m_RollmesNoF,m_Headmes)== true)
	{
		g_SampleReady = true;
	}

}

void CQuadcopterController::IrqLogTimerHandler()
{
	//Serial.println("TestLogTimer");

	if(GetControlLogState()==LOG_CONTROL_RECORD)
	{
		if(m_bAutoThrust)
		{
			if(m_AutoThrustCount >= m_AutoThrustTimeOut)
			{
				m_AutoThrustCount=0;
				//m_PWM = m_AutoThrust[1];
				//m_bAutoThrust = false;

				if(m_AutoThrustc>=9)
				{
					m_AutoThrustc=0;
					m_bAutoThrust = 0;
				}
				else
				{
					m_AutoThrustc++;
				}
				m_PWM = m_AutoThrust[m_AutoThrustc];

			}
			// Count up from log interval
			m_AutoThrustCount += (float)(m_LogControllerInterval/1000000.0);
		}

		LogHandle();

		if(!m_pQuadLog->SetControlItem
		(
			m_LogVars->f1,
			m_LogVars->f2,
			m_LogVars->f3,
			m_LogVars->f4,
			m_LogVars->f5,
			m_LogVars->f6,
			m_LogVars->f7,
			m_LogVars->f8,
			m_LogVars->f9,
			m_LogVars->f10,
			m_LogVars->f11,
			m_LogVars->f12
		)
		)
		{
			m_AutoThrustc=0;
			m_AutoThrustCount=0;
			EndLogTimer();
			m_bInterLoggerDone = true;
		}
	}
	else if(GetControlLogState()==LOG_CONTROL_PRINT)
	{

		if(GetControlLogType()==TYPE_CONTROL_PITCH_ROLL_ALTITUDE)
		{
			ControllerTransmitAngles();
		}
		else if(GetControlLogType()==TYPE_CONTROL_RAW_MEMS)
		{
			ControllerTransmitMemsRaw();
		}
		else if(GetControlLogType()==TYPE_CONTROL_LOG_INTERNAL)
		{
			ControllerTransmitInternalControlValues();
		}
		else if(GetControlLogType()==TYPE_CONTROL_FORCES)
		{
			ControllerTransmitForces();
		}
	}

}

void CQuadcopterController::InitSampleTimer()
{
	m_pSampleTimer->begin(IrqSampleTimer,(unsigned int)(1666));
}

void CQuadcopterController::InitLogTimer()
{

	m_pLogTimer->begin(IrqLogTimer,(unsigned int)(m_LogControllerInterval));
}

void CQuadcopterController::EndLogTimer()
{
	m_pLogTimer->end();
}

bool CQuadcopterController::SetControlLogState(uint8_t State)
{
	switch(State)
		{
			case 0: m_eControlLogState=LOG_CONTROL_NON;		    return true;
			case 1: m_eControlLogState=LOG_CONTROL_REAL_TIME; 	return true;
			case 2: m_eControlLogState=LOG_CONTROL_RECORD; 		return true;
			case 3: m_eControlLogState=LOG_CONTROL_PRINT;    	return true;
			case 4: m_eControlLogState=LOG_CONTROL_CAL;    	    return true;
			default:
				return false;
		}
}

bool CQuadcopterController::SetControlLogType(uint8_t State)
{
	switch(State)
		{
			case 0: m_eControlLogType = TYPE_CONTROL_PITCH_ROLL_ALTITUDE;	return true;
			case 1: m_eControlLogType = TYPE_CONTROL_RAW_MEMS; 				return true;
			case 2: m_eControlLogType = TYPE_CONTROL_LOG_INTERNAL; 			return true;
			case 3: m_eControlLogType = TYPE_CONTROL_FORCES; 				return true;
			default:
				return false;
		}
}

CQuadcopterController::EControlLogState CQuadcopterController::GetControlLogState()
{
	return m_eControlLogState;
}

CQuadcopterController::EControlLogType CQuadcopterController::GetControlLogType()
{
	return m_eControlLogType;
}

//----------------------------------------------------

bool CQuadcopterController::SetControlCom(uint8_t iCase)
{
	/*
	delay(2000);
	Serial1.println("SetControlCom() is called");
	delay(2000);
	*/

	switch(iCase)
	{
		case 0: m_eControl=CONTROL_COM;		  	  		return true;
		case 1: m_eControl=CONTROL_END;  			    return true;
		case 2: m_eControl=CONTROL_SETTINGS;      		return true;
		case 3: m_eControl=CONTROL_START;         		return true;
		case 4: m_eControl=CONTROL_TRANSMIT;      		return true;
		case 5: m_eControl=CONTROLLER_CALC_ANGLES; 		return true;
		case 6: m_eControl=CONTROLLER_TRANSMIT_ANGLES;  return true;


		default:
			return false;
	}
}

void CQuadcopterController::ControlCom()
{

	uint8_t iCase=0;
	if(m_pQuadcopterVisual->Com(iCase,m_eInterface))
	{
		SetControlCom(iCase);
	}
}

void CQuadcopterController::Control()
{

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Control Run\r\n",18,m_eInterface);
#endif

	while(1)
	{
		switch(m_eControl)
		{
			case CONTROL_COM:                ControlCom();           		  break;
			case CONTROL_SETTINGS:       	 ControlSettings();      		  break;
		    case CONTROL_START:          	 Controller();           		  break;
		    case CONTROL_TRANSMIT:       	 ControllerTransmit();   		  break;
		    case CONTROLLER_CALC_ANGLES:	 ControllerCalcAngles(); 		  break;
		    case CONTROLLER_TRANSMIT_ANGLES: ControllerTransmitAngles();  /* delay(1000); Serial1.print("m_eControl="); Serial1.println(m_eControl); Serial1.print("...2..."); delay(1000)*/;break;
			case CONTROL_END: 	    SetCom(CONTROLLER_COM);  SetControlCom(CONTROL_COM); return;
		}
	}
}

void CQuadcopterController::ControllerTransmitAngles()
{

	float r=0;
	float p=0;
	float a=0;

	char aItem[25];

	r=m_pConP->Rmesf;
	p=m_pConP->Pmesf;
	a=m_Hoower;

	int ItemSize = sprintf(aItem,"%f %f %f",r,p,a);
	m_pQuadcopterVisual->Write(aItem,ItemSize,m_eInterface);

}

void CQuadcopterController::ControllerTransmitForces()
{
	char aItem[10];
	int ItemSize = sprintf(aItem,"%d\r\n",m_Thrustmes);
	m_pQuadcopterVisual->Write(aItem,ItemSize,m_eInterface);
}

void CQuadcopterController::ControllerTransmitInternalControlValues()
{
	float r=m_pConP->Rmesf;
	float p=m_pConP->Pmesf;
	float a=m_Hoower;

	float er  = m_CRoll->e;
	float upr = m_CRoll->up;
	float udr = m_CRoll->ud;
	float uir = m_CRoll->ui;

	float ep  = m_CPitch->e;
	float upp = m_CPitch->up;
	float udp = m_CPitch->ud;
	float uip = m_CPitch->ui;

	float eh  = m_CHoower->e;
	float uph = m_CHoower->up;
	float udh = m_CHoower->ud;
	float uih = m_CHoower->ui;

	char aItem[200];
	int ItemSize = sprintf(aItem,"Angles:   %.1f   %.1f   %.1f   Roll:  %.1f   %.1f   %.1f   %.1f   Pitch:  %.1f   %.1f   %.1f   %.1f  Altitude: %.1f   %.1f   %.1f   %.1f",r,p,a,er,upr,udr,uir,ep,upp,udp,uip,eh,uph,udh,uih);
	m_pQuadcopterVisual->Write(aItem,ItemSize,m_eInterface);

}

void CQuadcopterController::ControllerTransmitMemsRaw()
{
	char aItem[300];
	int ItemSize = sprintf(aItem,"Gyro xyz: %.1f   %.1f   %.1f   Acc xxy:  %.1f   %.1f   %.1f",m_lxg,m_lyg,m_lzg,m_lxa,m_lya,m_lza);
	m_pQuadcopterVisual->Write(aItem,ItemSize,m_eInterface);
}


void CQuadcopterController::ControllerCalcAngles()
{
	m_pSampleTimer->end();
	m_pQuadCopterFeedBack->Reset();

	float fpitch = 0;
	float froll = 0;
	float pitch = 0;
	float roll = 0;
	float heading =0;
	float Sumpitch =0;
	float Sumroll =0;

	int avgNum = 50.0;

	for(int i=0; i<avgNum; i++)
	{
		GetFeedbackCal(fpitch,froll,pitch,roll,heading);
		Sumpitch +=fpitch;
		Sumroll  +=froll;
	    delay(5);
	}


	char aItem[25];
	int ItemSize = sprintf(aItem,"Pitch=%f Roll=%f",pitch/avgNum,roll/avgNum);
	m_pQuadcopterVisual->Write(aItem,ItemSize,m_eInterface);

	InitSampleTimer();

	SetControlCom(CONTROL_END);
}

void CQuadcopterController::Controller()
{

	while(1)
	{
		if(g_SampleReady)
		{
			g_SampleReady = false;

			//if(m_MotorState)
			if(1)
			{
				//m_pQuadSpeedControl->SetSpeedBin()UpdateSpeed(Ms2Pwm(m_PWM),Ms2Pwm(m_PWM),Ms2Pwm(m_PWM),Ms2Pwm(m_PWM));
			}
			else
			{
				m_pQuadSpeedControl->SetSpeedControlleroff();
			}

			// update log registers
			//LogHandle();

			if(GetControlLogState()==LOG_CONTROL_REAL_TIME)
			{
				LogHandle();
				if(!m_pQuadLog->SetControlItem
					(
						m_LogVars->f1,
						m_LogVars->f2,
						m_LogVars->f3,
						m_LogVars->f4,
						m_LogVars->f5,
						m_LogVars->f6,
						m_LogVars->f7,
						m_LogVars->f8,
						m_LogVars->f9,
						m_LogVars->f10,
						m_LogVars->f11,
						m_LogVars->f12
					)
				)
				{

					SetControlCom(CONTROL_TRANSMIT);
					break;
				}
			}
			else if(GetControlLogState()==LOG_CONTROL_RECORD)
			{
				if(m_bInterLoggerDone==true)
				{
					m_bInterLoggerDone = false;
					SetControlCom(CONTROL_TRANSMIT);
					break;
				}

			}

		}

		// Interface listener
		m_pQuadcopterVisual->ReadFast(m_aMessage,m_iReadSize,m_eInterface);

		if(m_iReadSize>0)
		{
			if(InterpretComControl(m_aMessage,m_iReadSize))
			{
				break;
			}
		}


#ifdef CONTROLLER_DEBUG
		delay(100);
#endif

	}
}

void CQuadcopterController::LogHandle()
{
	// Set desired log varibles f1-f12 registers

	if(GetControlLogType()==TYPE_CONTROL_PITCH_ROLL_ALTITUDE || GetControlLogType()== TYPE_CONTROL_LOG_INTERNAL)
	{
		m_LogVars->f1  = m_Rollmes;
		m_LogVars->f2  = m_CRoll->up;
		m_LogVars->f3  = m_CRoll->ui;
		m_LogVars->f4  = m_CRoll->ud;
		m_LogVars->f5  = m_Pitchmes;
		m_LogVars->f6  = m_CPitch->up;
		m_LogVars->f7  = m_CPitch->ui;
		m_LogVars->f8  = m_CPitch->ud;
		m_LogVars->f9  = m_Hoowermes;
		m_LogVars->f10 = m_CHoower->up;
		m_LogVars->f11 = m_CHoower->ui;
		m_LogVars->f12 = m_CHoower->ud;
	}
	else if(GetControlLogType()==TYPE_CONTROL_RAW_MEMS)
	{
		m_pQuadCopterFeedBack->GetSampleRaw(m_lxg,m_lyg,m_lzg,m_lxa,m_lya,m_lza);
		m_LogVars->f1  = m_lxg;
		m_LogVars->f2  = m_lyg;
		m_LogVars->f3  = m_lzg;
		m_LogVars->f4  = m_lxa;
		m_LogVars->f5  = m_lya;
		m_LogVars->f6  = m_lza;
		m_LogVars->f7  = m_Rollmes;
		m_LogVars->f8  = m_Pitchmes;
		m_LogVars->f9  = m_Hoowermes;
		m_LogVars->f10 = 2;
		m_LogVars->f11 = 2;
		m_LogVars->f12 = 2;
	}
	else if(GetControlLogType()==TYPE_CONTROL_FORCES)
	{
		m_LogVars->f1  = 0;
		m_LogVars->f2  = 0;
		m_LogVars->f3  = 0;
		m_LogVars->f4  = 0;
		m_LogVars->f5  = 0;
		m_LogVars->f6  = 0;
		m_LogVars->f7  = 0;
		m_LogVars->f8  = 0;
		m_LogVars->f9  = 0;
		m_LogVars->f10 = 0;
		m_LogVars->f11 = 0;
		m_LogVars->f12 = 0;
	}
}

void CQuadcopterController::ControllerTransmit()
{

	bool m_bLog=true;
	char  Item[500];
	uint32_t iReadSize=0;

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Start Control Log\r\n",12,m_eInterface);
#endif


	m_pQuadSpeedControl->SetSpeedControlleroff();


	//TransmitParam(m_ThrustmesOffSet);

	delay(10);

	while(m_bLog)
	{

		if(m_pQuadLog->GetItemControl(Item,iReadSize))
		{

			m_pQuadcopterVisual->Write(Item,iReadSize,m_eInterface);
			delay(10);
		}
		else
		{
			delay(10);
			SetControlCom(CONTROL_END);
			return;

		}

	}

}

void CQuadcopterController::TransmitParam(uint16_t ThrustmesOffSet)
{
	Serial.print(ThrustmesOffSet);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.print(0);
	Serial.print(",");
	Serial.println(0);

}

void CQuadcopterController::ResetLogControl()
{

}

void CQuadcopterController::ControlSettings()
{

#ifdef CONTROLLER_DEBUG
	m_pQuadcopterVisual->Write("Control Settings\r\n",18,m_eInterface);
#endif

	m_pQuadcopterVisual->Read(m_aMessage,m_iReadSize,m_eInterface);
	SetControlSitting(m_aMessage,m_iReadSize);
}

bool CQuadcopterController::SetControlSitting(char * pMessage, uint8_t iReadSize)
{
	float aArg[8];
	uint8_t iArg=0;
	if(m_pQuadcopterVisual->Validate(pMessage,iReadSize,"CONSET"))
	{
		if(m_pQuadcopterVisual->FecthArgsFloat(pMessage,iReadSize,aArg,iArg))
		{
			if(iArg==8)
			{
				SetControlParam(aArg);
				return true;
			}
		}
	}
	return false;
}

bool CQuadcopterController::InterpretComControl(char * pMessage, uint8_t iReadSize)
{
	if(pMessage[0]=='z')
	{
		m_pQuadSpeedControl->SetSpeedControlleroff();

		EndLogTimer();

		SetControlCom(CONTROL_END);

		return true;
	}

	uint32_t aArg[2];
	float aArgf[3];
	uint8_t iArg=0;

	if(m_pQuadcopterVisual->Validate(pMessage,iReadSize,"CONP"))
	{
		if(m_pQuadcopterVisual->FecthArgs(pMessage,iReadSize,aArg,iArg))
		{
			if(iArg==2)
			{
				SetHoowerStep(aArg[0],aArg[1]);
			}
		}
	}
	else if(m_pQuadcopterVisual->Validate(pMessage,iReadSize,"FC"))
	{
		if(m_pQuadcopterVisual->FecthArgsFloat(pMessage,iReadSize,aArgf,iArg))
		{
			if(iArg==3)
			{
				SetReferences(aArgf[0],aArgf[1],aArgf[2]);
			}
		}
	}
	else
	{
		Serial.println("Failed");
	}
	return false;
}

bool CQuadcopterController::SetControlParam(float *pArg)
{
	InitRollPIDController();
	InitPitchPIDController();
	InitHoowerPIDController();

	m_CLogParam->LogState   = pArg[0];
	m_CLogParam->LogType    = pArg[1];
    m_LogControllerInterval = pArg[2];
    m_CLogParam->LogNum     = pArg[3];
    m_PWM     				= pArg[4];
    m_bAutoThrust           = pArg[5];
    m_AutoThrustTimeOut     = pArg[6];
    m_StepPWMVaule     		= pArg[7];

    //Serial.println("Log num is");
    //Serial.print(m_CLogParam->LogNum);
    //InitAutoThrust();

    //ControllerCalcForce();

    //UpdateSpeed(Ms2Pwm(m_PWM),Ms2Pwm(m_PWM),Ms2Pwm(m_PWM),Ms2Pwm(m_PWM));


    // 2,3, 1.000.000 , 10, 1.2 , 0, 3

    SetControlLogState(m_CLogParam->LogState);

    if(GetControlLogState() == LOG_CONTROL_CAL)
   	{
   		SetControlCom(CONTROLLER_CALC_ANGLES);
   		return 1;
   	}

    if(GetControlLogState() > LOG_CONTROL_REAL_TIME)
   	{
   		InitLogTimer();
   	}

    SetControlLogType(m_CLogParam->LogType);

   	m_pQuadLog->SetItemCountControl(m_CLogParam->LogNum);


   	SetControlCom(CONTROL_START);

	/*
	Serial.print("LogState=");
	Serial.println(m_CLogParam->LogState );

	Serial.print("type=");
	Serial.println(m_CLogParam->LogType);

	Serial.print("SampleRate=");
	Serial.println(m_LogControllerInterval);

	Serial.print("LogNum=");
	Serial.println(m_CLogParam->LogNum);
	*/


}
void CQuadcopterController::SetHoower(uint16_t iCHooverWeight)
{
	if(m_CHooverWeight > m_CSat->SatHighHoow)
	{
		m_CHooverWeight = m_CSat->SatHighHoow;
	}
	else if(m_CHooverWeight < m_CSat->SatLowHoow)
	{
		m_CHooverWeight = m_CSat->SatLowHoow;
	}
	else
	{
		m_CHooverWeight =iCHooverWeight;
	}
}
void CQuadcopterController::HoowerSaturation(float &UHoower)
{
	if(UHoower > m_CSat->SatHighHoow)
	{
		UHoower = m_CSat->SatHighHoow;
	}
	else if(UHoower < m_CSat->SatLowHoow)
	{
		UHoower = m_CSat->SatLowHoow;
	}
}

void CQuadcopterController::HoowerSaturation2(float &UHoower)
{
	if(UHoower > 40000)
	{
		UHoower = 40000;
	}
	else if(UHoower < 21500)
	{
		UHoower = 21500;
	}
}

void CQuadcopterController::SetHoowerStep(uint8_t iCHooverWeight,uint8_t iDirection)
{
	if(iDirection==1)
	{
		if(m_CHooverWeight + iCHooverWeight > m_CSat->SatHighHoow)
		{
			m_CHooverWeight = m_CSat->SatHighHoow;
		}
		else
		{
			m_CHooverWeight += iCHooverWeight;
		}

	}
	else if(iDirection==0)
	{
		if(m_CHooverWeight - iCHooverWeight < m_CSat->SatLowHoow)
		{
			m_CHooverWeight = m_CSat->SatLowHoow;
		}
		else
		{
			m_CHooverWeight -= iCHooverWeight;
		}
	}


}
void CQuadcopterController::SetReferences(float Pitch, float Roll, float Altitude)
{
	m_CRoll->ref = Roll;
	m_CPitch->ref = Pitch;
}
void CQuadcopterController::InitControllerSaturation()
{
	m_CSat->SatHigh = 39000;
	m_CSat->SatLow  = 21500;
	m_CSat->SatHighHoow = 800;
	m_CSat->SatLowHoow  = 150;
}
void CQuadcopterController::InitHoowerPIDController()
{
	m_CHoower->Kp = 5.9;
	m_CHoower->Kd = 0.031;
	m_CHoower->Ki = 1.03;
	m_CHoower->udLast=0;
	m_CHoower->uILast=0;

	m_CHoower->up=0;
	m_CHoower->ud=0;
	m_CHoower->ui=0;
	m_CHoower->e =0;
	m_CHoower->uiTres =4;
	m_CHoower->ref = 15 ;
	m_CHoower->State = false ;

	m_pQuadSpeedControl->SetSpeedControlleroff();
}
void CQuadcopterController::InitRollPIDController()
{
	m_CRoll->Kp = 2.9;
	m_CRoll->Kd = 0.31;
	m_CRoll->Ki = 1.03;
	m_CRoll->udLast=0;
	m_CRoll->uILast=0;

	m_CRoll->up=0;
	m_CRoll->ud=0;
	m_CRoll->ui=0;
	m_CRoll->e =0;
	m_CRoll->uiTres =4;
	m_CRoll->ref = 0 ;
	m_CRoll->State =1;

	m_pQuadSpeedControl->SetSpeedControlleroff();
}
void CQuadcopterController::InitPitchPIDController()
{
	m_CPitch->Kp = 2.9;
	m_CPitch->Kd = 0.31;
	m_CPitch->Ki = 1.03;
	m_CPitch->udLast=0;
	m_CPitch->uILast=0;
	m_CPitch->State =1;

	m_CPitch->up=0;
	m_CPitch->ud=0;
	m_CPitch->ui=0;
	m_CPitch->e =0;
	m_CPitch->uiTres =4;
	m_CPitch->ref = 0;

	m_pQuadSpeedControl->SetSpeedControlleroff();

}
void CQuadcopterController::InitControlLog()
{
	m_CLogParam->LogState = 0;
	m_lxg=0;
	m_lyg=0;
	m_lzg=0;
	m_lxa=0;
	m_lya=0;
	m_lza=0;
}
void CQuadcopterController::RollPIDController(float MRoll, float &URoll)
{

	// error
	m_CRoll->e = m_CRoll->ref - MRoll;

#ifdef CONTROLLER_DEBUG
	Serial.print(" e= ");
	Serial.print(m_CRoll->e,3);
#endif

	// Proportional
	m_CRoll->up = m_CRoll->e * m_CRoll->Kp;

#ifdef CONTROLLER_DEBUG
	Serial.print(" Up= ");
	Serial.print(m_CRoll->up);
#endif

	// Differential
	m_CRoll->ud =  (m_CRoll->up - m_CRoll->udLast)*600*m_CRoll->Kd;
	m_CRoll->udLast     =   m_CRoll->up;

#ifdef CONTROLLER_DEBUG
	Serial.print(" Ud= ");
	Serial.print(m_CRoll->ud,6);
#endif

	//Serial.print(" ");
	//Serial.print("up-udLast= ");
	//Serial.print(m_CRoll->up - m_CRoll->udLast);

	// Integrator
	m_CRoll->ui = (float)(m_CRoll->up*0.0016666) + m_CRoll->uILast;

	if(m_CRoll->ui >= m_CRoll->uiTres)
	{
		m_CRoll->ui = m_CRoll->uiTres;
		m_CRoll->uILast =0;
	}
	else if(m_CRoll->ui <= -m_CRoll->uiTres)
	{
		m_CRoll->ui = -m_CRoll->uiTres;
		m_CRoll->uILast =0;
	}
	else
	{
		m_CRoll->uILast += m_CRoll->ui;
	}

	m_CRoll->ui *=  m_CRoll->Ki;


#ifdef CONTROLLER_DEBUG
	Serial.print(" Ui= ");
	Serial.print(m_CRoll->ui,6);
#endif
	//Serial.print(" uILast= ");
	//Serial.println(m_CRoll->uILast,6);

	// Output
	URoll = m_CRoll->up + m_CRoll->ud + m_CRoll->ui;

#ifdef CONTROLLER_DEBUG
	Serial.print(" U= ");
	Serial.print(URoll,6);
#endif

}
void CQuadcopterController::PitchPIDController(float MPitch, float &UPitch)
{
	// error
	m_CPitch->e = m_CPitch->ref - MPitch;

#ifdef CONTROLLER_DEBUG
	Serial.print(" e= ");
	Serial.print(m_CPitch->e,3);
#endif

	// Proportional
	m_CPitch->up = m_CPitch->e * m_CPitch->Kp;

#ifdef CONTROLLER_DEBUG
	Serial.print(" Up= ");
	Serial.print(m_CPitch->up);
#endif

	// Differential
	m_CPitch->ud =  (m_CPitch->up - m_CPitch->udLast)*600*m_CPitch->Kd;
	m_CPitch->udLast     =   m_CPitch->up;

#ifdef CONTROLLER_DEBUG
	Serial.print(" Ud= ");
	Serial.print(m_CPitch->ud,6);
#endif

	//Serial.print(" ");
	//Serial.print("up-udLast= ");
	//Serial.print(m_CPitch->up - m_CPitch->udLast);

	// Integrator
	m_CPitch->ui = (float)(m_CPitch->up*0.0016666) + m_CPitch->uILast;

	if(m_CPitch->ui >= m_CPitch->uiTres)
	{
		m_CPitch->ui = m_CPitch->uiTres;
		m_CPitch->uILast =0;
	}
	else if(m_CPitch->ui <= -m_CPitch->uiTres)
	{
		m_CPitch->ui = -m_CPitch->uiTres;
		m_CPitch->uILast =0;
	}
	else
	{
		m_CPitch->uILast += m_CPitch->ui;
	}

	m_CPitch->ui *= m_CPitch->Ki;

#ifdef CONTROLLER_DEBUG
	Serial.print(" Ui= ");
	Serial.print(m_CPitch->ui,6);
#endif
	//Serial.print(" uILast= ");
	//Serial.println(m_CPitch->uILast,6);

	// Output
	UPitch = m_CPitch->up + m_CPitch->ud + m_CPitch->ui;

#ifdef CONTROLLER_DEBUG
	Serial.print(" U= ");
	Serial.print(UPitch,6);
#endif

}
void CQuadcopterController::HoowerPIDController(float MHoower, float &UHoower)
{
	// error
	m_CHoower->e = m_CHoower->ref - MHoower;

	//Serial.print(" e= ");
	//Serial.print(m_CHoower->e,3);

#ifdef CONTROLLER_DEBUG
	Serial.print(" e= ");
	Serial.print(m_CHoower->e,3);
#endif

	// Proportional
	m_CHoower->up = m_CHoower->e * m_CHoower->Kp;

#ifdef CONTROLLER_DEBUG
	Serial.print(" Up= ");
	Serial.print(m_CHoower->up);
#endif

	// Differential
	m_CHoower->ud =  (m_CHoower->up - m_CHoower->udLast)*600*m_CHoower->Kd;
	m_CHoower->udLast     =   m_CHoower->up;

#ifdef CONTROLLER_DEBUG
	Serial.print(" Ud= ");
	Serial.print(m_CHoower->ud,6);
#endif

	//Serial.print(" ");
	//Serial.print("up-udLast= ");
	//Serial.print(m_CHoower->up - m_CHoower->udLast);

	// Integrator
	m_CHoower->ui = (float)(m_CHoower->up*0.0016666) + m_CRoll->uILast;

	if(m_CHoower->ui >= m_CHoower->uiTres)
	{
		m_CHoower->ui = m_CHoower->uiTres;
		m_CHoower->uILast =0;
	}
	else if(m_CHoower->ui <= -m_CHoower->uiTres)
	{
		m_CHoower->ui = -m_CHoower->uiTres;
		m_CHoower->uILast =0;
	}
	else
	{
		m_CHoower->uILast += m_CHoower->ui;
	}

	m_CHoower->ui *= m_CHoower->Ki;

#ifdef CONTROLLER_DEBUG
	Serial.print(" Ui= ");
	Serial.print(m_CHoower->ui,6);
#endif
	//Serial.print(" uILast= ");
	//Serial.println(m_CRoll->uILast,6);

	// Output
	UHoower = m_CHoower->up + m_CHoower->ud + m_CHoower->ui + 175;

#ifdef CONTROLLER_DEBUG
	Serial.print(" U= ");
	Serial.println(UHoower,6);
#endif

}
void CQuadcopterController::XConfigurationMixer(
		float URoll,
		float UPitch,
		float UHoower,
		float &M1,
		float &M2,
		float &M3,
		float &M4
)
{
	HoowerSaturation(UHoower);

	//Serial.print(" Trust out= ");
	//Serial.println((int32_t)(UHoower));
	//delay(20);

#ifdef CONTROLLER_DEBUG_M
	Serial.print(" M1T= ");
    Serial.print( -URoll  - UPitch + UHoower);
    Serial.print(" M2T= ");
    Serial.print( -URoll  + UPitch + UHoower);
    Serial.print(" M3T= ");
    Serial.print( URoll  - UPitch + UHoower);
    Serial.print(M3);
    Serial.print(" M4T= ");
    Serial.print( URoll  + UPitch + UHoower);
#endif


    M1 = Thrust2PWM( -URoll  - UPitch + UHoower);
    M2 = Thrust2PWM( -URoll  + UPitch + UHoower);
    M3 = Thrust2PWM(  URoll  - UPitch + UHoower);
    M4 = Thrust2PWM(  URoll  + UPitch + UHoower);



}
void CQuadcopterController::XConfigurationMixer2
(
	float URoll,
	float UPitch,
	float UHoower,
	float &M1,
	float &M2,
	float &M3,
	float &M4
)
{
	HoowerSaturation2(UHoower);
	//Serial.println((int32_t)(UHoower));
	//delay(20);

#ifdef CONTROLLER_DEBUG_M
	Serial.print(" M1T= ");
    Serial.print( -URoll  - UPitch + UHoower);
    Serial.print(" M2T= ");
    Serial.print( -URoll  + UPitch + UHoower);
    Serial.print(" M3T= ");
    Serial.print( URoll  - UPitch + UHoower);
    Serial.print(M3);
    Serial.print(" M4T= ");
    Serial.print( URoll  + UPitch + UHoower);
#endif

    M1 =  -URoll  - UPitch + UHoower;
    M2 =  -URoll  + UPitch + UHoower;
    M3 =   URoll  - UPitch + UHoower;
    M4 =   URoll  + UPitch + UHoower;


}
float CQuadcopterController::Thrust2PWM2(float Thrust)
{
	float pwm = (Thrust+841.6511)/0.0389;

	if(pwm>39000)
	{
		pwm =39000;
	}
	else if(pwm<21500)
	{
		pwm =21500;
	}
	return pwm;
}
float CQuadcopterController::Thrust2PWM(float Thrust)
{
	float ms = (Thrust+711.72)/656.55;

	if(ms>2)
	{
		ms=2;
	}
	else if(ms<1)
	{
		ms=1;
	}
	return  ms*19602;
}

float CQuadcopterController::Thrust2DutyMS(float Thrust)
{
	float ms = (Thrust-6.9797)/6.4386;
	if(ms>2)
	{
		ms=2;
	}
	else if(ms<1)
	{
		ms=1;
	}
	return ms;
}

