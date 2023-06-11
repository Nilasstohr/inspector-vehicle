/*
 * TestVehiclePIControl.cpp
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#include "TestVehiclePIControl.h"

static CSyncEvent g_SampleEventTimerControl;

extern "C" void IrqSampleTimerControl(void){
	g_SampleEventTimerControl();
}

TestVehiclePIControl::TestVehiclePIControl() {
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "############# TEST CLASS ############# ");

	this->timerIntervalUs = 100;
	this->timerIntervalSeconnds = (double)( this->timerIntervalUs/(pow(10,6)) );
	this->timerIntervalMillis = (double)( this->timerIntervalUs/(pow(10,3)) );
	this->averageSize = 5;

	this->motorDrivers = buildMotorDrivers();
	this->quadratureEncoders = buildQuadratureEncoders();
	this->encoders()->setupEncoders();

	this->sampleTimer = new IntervalTimer;
	this->averageCalcLeft = new AverageCalc(getAverageSize());
	this->averageCalcRight = new AverageCalc(getAverageSize());
	this->timeSeconds = 0;
	g_SampleEventTimerControl.Bind(this,&TestVehiclePIControl::sampleEventTimerHandler);

	canInitializePiController();
	canPerformPiControl();
}

void TestVehiclePIControl::canInitializePiController(){
	Logger::verbose(__FUNCTION__, "- TEST");
	this->piControllerLeft =  new TransposedIIRFilter(
			VEHICLE_PI_CONTROL_COEFFICIENT_B0,
			VEHICLE_PI_CONTROL_COEFFICIENT_B1,
			VEHICLE_PI_CONTROL_COEFFICIENT_A1);
	this->piControllerRight = new TransposedIIRFilter(
			VEHICLE_PI_CONTROL_COEFFICIENT_B0,
			VEHICLE_PI_CONTROL_COEFFICIENT_B1,
			VEHICLE_PI_CONTROL_COEFFICIENT_A1);
}

void TestVehiclePIControl::canPerformPiControl() {
	Logger::verbose(__FUNCTION__, "- TEST");


	uint8_t  timeRunSeconds = 3;

	uint32_t bufferSize = (uint32_t)(
			timeRunSeconds*pow(10,6) / (getTimerIntervalUs()*getAverageSize())
	);

	Serial.println("Timer Parameters");
	Serial.println("---------------------");
	Serial.print("Run time s:");
	Serial.println(timeRunSeconds);
	Serial.print("Interval in us:");
	Serial.println(getTimerIntervalUs());
	Serial.print("Interval in ms:");
	Serial.println(getTimerIntervalMillis(),1);
	Serial.print("Interval in s:");
	Serial.println(getTimeIntervalSeconds(),4);
	Serial.print("buffer size:");
	Serial.println(bufferSize);



	double dataLeft[bufferSize];
	double dataRight[bufferSize];


	if(!sampleTimer->begin(IrqSampleTimerControl,(unsigned int)(getTimerIntervalUs()))){
		Logger::error("invalid timer interval");
		return;
	}

	Logger::verbose("timer started");


	double omegaLeft = 0;
	double omegaRight = 0;
	double refVelocity = 10;
	//double feedForward = 0;
	//double feedBack = 0;

	uint32_t actualTicks = 0;

	uint32_t tic;
	uint32_t toc;
	tic = micros();
	while(actualTicks < bufferSize){
		if(ready)
		{
			actualTicks++;
			//Serial.println(actualTicks);

			omegaLeft  = getAngularVelocity(this->intervalLeft);
			omegaRight = getAngularVelocity(this->intervalRight);

			dataLeft[actualTicks] = omegaLeft;
			dataRight[actualTicks]= omegaRight;

			motors()->forward(
				(uint16_t)this->piControllerLeft->update(refVelocity-omegaLeft),
				(uint16_t)this->piControllerRight->update(refVelocity-omegaRight)
			);

			//Serial.print(this->intervalLeft);
			//Serial.print(",");
			//Serial.println(this->intervalRight);

			//Serial.print(omegaLeft);
			//Serial.print(",");
			//Serial.println(omegaRight);

			ready = false;
		}


		delayMicroseconds(1);

    }
	toc = micros();
	sampleTimer->end();
	motors()->forward(0);

	Serial.print("tic/toc in seconds was:");
	double actualRunTime = (double)( (toc-tic)/pow(10,6) );
	Serial.println(actualRunTime);
	Serial.print("timer sum in seconds was:");
	Serial.println(getTimeInSeconds());


	Serial.println("---------done printing data--------");
	for(uint16_t i=0; i<actualTicks; i++){
		Serial.print(dataLeft[i],3);
		Serial.print(" ");
		Serial.println(dataRight[i],3);
	}

}
double TestVehiclePIControl::getAngularVelocity(uint32_t interval) {
	if(interval==0){
		return 0;
	}
	//Serial.println("non zero velocity");
	return encoders()->left()->getParameters()->calculateAngularVelocity(interval);

}

uint32_t TestVehiclePIControl::getTimeInterval(QuadratureEncoders::QuadratureEncoderSide side) {
	uint32_t interval =  encoders()->encoder(side)
			 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros);
	if(interval==0){
		return interval;
	}
	if(interval < 85){
		interval =  85;
	}
	return interval;
}


void TestVehiclePIControl::sampleEventTimerHandler() {
	getAverageCalcLeft()->add(getTimeInterval(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left));

	/*
	getAverageCalcLeft()->add(getTimeInterval(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left));
	if(getAverageCalcRight()->add(getTimeInterval(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right))){
		this->intervalLeft  = getAverageCalcLeft()->average();
		this->intervalRight = getAverageCalcRight()->average();
		//Serial.print(this->intervalLeft);
		//Serial.print(" ");
		//Serial.println(this->intervalRight);
		ready=true;
	}
	*/
	this->timeSeconds+=getTimeIntervalSeconds();
	this->timeMillis +=getTimerIntervalMillis();
}


TestVehiclePIControl::~TestVehiclePIControl() {
	// TODO Auto-generated destructor stub
}

AverageCalc* TestVehiclePIControl::getAverageCalcLeft() {
	return this->averageCalcLeft;
}


AverageCalc* TestVehiclePIControl::getAverageCalcRight() {
	return this->averageCalcRight;
}


unsigned int TestVehiclePIControl::getTimerIntervalUs() {
	return this->timerIntervalUs;
}

uint8_t TestVehiclePIControl::getAverageSize() {
	return this->averageSize;
}

double TestVehiclePIControl::getTimeIntervalSeconds() {
	return this->timerIntervalSeconnds;
}

double TestVehiclePIControl::getTimerIntervalMillis() {
	return this->timerIntervalMillis;
}

double TestVehiclePIControl::getTimeInMillis() {
	return this->timeMillis;
}

double TestVehiclePIControl::getTimeInSeconds() {
	return this->timeSeconds;
}

