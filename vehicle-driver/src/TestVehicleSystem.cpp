/*
 * TestVehicleSystem.cpp
 *
 *  Created on: Sep 15, 2022
 *      Author: robot1
 */

#include "TestVehicleSystem.h"

static CSyncEvent g_SampleEventTimer;

extern "C" void IrqSampleTimer(void){
	g_SampleEventTimer();
}

TestVehicleSystem::TestVehicleSystem() {
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "############# TEST CLASS ############# ");

	this->motorDrivers = createMotorDrivers();
	this->quadratureEncoders = createQuadratureEncoders();
	this->encoders()->setupEncoders();

	// set up sample timer
	this->sampleTimer = new IntervalTimer;
	g_SampleEventTimer.Bind(this,&TestVehicleSystem::sampleEventTimerHandler);
	//attachInterrupt(this->quadratureEncoderRight->getParameters()->getPinChannelA(),channel_a_isr_2, CHANGE);

	// tests
	//testIncreasedEncoderCounts();
	//testDecreasedEncoderCounts();
	//testCanGetPositionForward();
	//testCanGetPositionBackWards();
	//testCanAngularVelocity();
	//testManualAngularVelocityCalc();
	//testCanDoEncoderRecord();
	testCanSampleEncoderRecords();
}

// Tests

void TestVehicleSystem::testIncreasedEncoderCounts() {
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->forward(30000);
	delay(1000);

	String * log;
	signed int count;
	for(int i=0;i<2;i++){
		log = this->count(count, (QuadratureEncoders::QuadratureEncoderSide)(i));
		if(count<=0){
			Logger::error(log->append(", not counting up as expected").c_str());
		}
	}
	motors()->stop();
}

void TestVehicleSystem::testDecreasedEncoderCounts() {
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->reverse(30000);
	delay(1000);

	String * log;
	signed int count;
	for(int i=0;i<2;i++){
		log = this->count(count, (QuadratureEncoders::QuadratureEncoderSide)(i));
		if(count>=0){
			Logger::error(log->append(", not counting down as expected").c_str());
		}
	}
	motors()->stop();
}

void TestVehicleSystem::testCanGetPositionForward() {
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->forward(30000);
	delay(1500);
    double position;
	for(int i=0;i<2;i++){
		position = encoders()->encoder((QuadratureEncoders::QuadratureEncoderSide)(i))->getPosition();
		Serial.println(position);
		if(position<=0){
			 String * log = new String();
			 log->append("encoder ")
					 .append(i)
					 .append(" of vehicle not moving forward, position is ")
					 .append(position)
					 .newLine();
			 Logger::error(log->c_str());
		}
	}
	motors()->stop();
}

void TestVehicleSystem::testCanGetPositionBackWards(){
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->reverse(30000);
	delay(1500);
	double position;
	for(int i=0;i<2;i++){
		position = encoders()->encoder((QuadratureEncoders::QuadratureEncoderSide)(i))->getPosition();
		Serial.println(position);
		if(position>=0){
			 String * log = new String();
			 log->append("encoder ")
					 .append(i)
					 .append(" of vehicle not moving forward, position is ")
					 .append(position)
					 .newLine();
			 Logger::error(log->c_str());
		}
	}
	motors()->stop();
}

void TestVehicleSystem::testCanAngularVelocity() {
	Logger::verbose(__FUNCTION__, "- TEST");
	this->resetTest();
	String * log = new String();
	double angularVelocity;
	uint32_t timeDiffMs;
	uint32_t tic;
	uint32_t toc;

	timeDiffMs= encoders()->encoder(QuadratureEncoders::quadrature_encoder_left)
							->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros);
	tic = micros();
	angularVelocity = encoders()->left()->getParameters()->calculateAngularVelocity(timeDiffMs);
	toc = micros();

	if(angularVelocity<=0)
		 Logger::error("of vehicle not moving forward, velocity was");
	Logger::verbose(log->append(angularVelocity).append(" rad/s, interval was ").append(timeDiffMs).append(" us calculation time was ").append(toc-tic).append(" us").c_str());

	motors()->stop();

}

void TestVehicleSystem::testManualAngularVelocityCalc() {
	Logger::verbose(__FUNCTION__, "- TEST");

	//this->resetTest();

	uint32_t tic;
	uint32_t toc;

	QuadratureEncorderParameters *  params = encoders()->left()->getParameters();

	double countToRadians  = params->getConstantCountsToRadians();
	uint32_t timeDiff = 82;

	Serial.print("countToRadianConstant=");
	Serial.println(countToRadians,6);
	Serial.print("timeDiff=");
	Serial.println(timeDiff);

	tic = micros();
	double theta_m = (double)(countToRadians/params->msToSeconds(timeDiff));
	toc = micros();
	Serial.print("angular velocity 1=");
	Serial.println(theta_m,3);

	Serial.print("calculation time 1=");
	Serial.println(toc-tic);


	tic = micros();
	theta_m = params->calculateAngularVelocity(timeDiff);
	toc = micros();
	Serial.print("angular velocity 2=");
	Serial.println(theta_m,3);
	Serial.print("calculation time 2=");
	Serial.println(toc-tic);

	tic = micros();
	theta_m = 0.000654/(timeDiff/pow(10,6));
	toc = micros();
	Serial.print("angular velocity 3=");
	Serial.println(theta_m,3);
	Serial.print("calculation time 3=");
	Serial.println(toc-tic);

	tic = micros();
	theta_m = params->calculateAngularVelocityFast(timeDiff);
	toc = micros();
	Serial.print("angular velocity 4=");
	Serial.println(theta_m,3);
	Serial.print("calculation time 4=");
	Serial.println(toc-tic);



	//motors()->stop();

}

void TestVehicleSystem::testCanDoEncoderRecord() {
	Logger::verbose(__FUNCTION__, "- TEST");
	this->resetTest();
	uint32_t tic;
	uint32_t toc;

	tic = micros();
	EncoderRecord * rec = new EncoderRecord(this->encoders()-> left());
	toc = micros();

	String * recStr = new String();
	rec->getRecord(recStr);
	recStr->space().append(toc-tic);
	Logger::verbose(recStr->c_str());
	motors()->stop();

}

void TestVehicleSystem::testCanSampleEncoderRecords() {
	Logger::verbose(__FUNCTION__, "- TEST");
    this->record.clear();
	Logger::verbose("staring timer");
	unsigned int interval = 500;
	if(!sampleTimer->begin(IrqSampleTimer,(unsigned int)(interval))){
		Logger::error("invalid timer interval");
		return;
	}
	motors()->forward(65000);
	//this->resetTest();
	delay(1000);
	sampleTimer->end();
	motors()->stop();
	Logger::verbose("ending timer");

	Logger::verbose("result is:");
	for(int i=0; i<record.size();i++){
		EncoderRecord r = this->record[i];
		//String * recStr = new String();
		//r.getRecord(recStr);
		Serial.println(r.getRadianPrSecond(),2);
		//Logger::verbose(__FUNCTION__,recStr->c_str());
	}
	this->record.clear();
}


// helpers
String *TestVehicleSystem::count(signed int &count,int side) {
	String * log = encoders()->count(count,(QuadratureEncoders::QuadratureEncoderSide)(side));
	Logger::verbose(__FUNCTION__,log->c_str());
	return log;
}

void TestVehicleSystem::sampleEventTimerHandler() {
	this->record.push_back(EncoderRecord(this->encoders()->left()));
}

void TestVehicleSystem::resetTest() {
	delay(2000);
	encoders()->reset();
	motors()->forward(65000);
	delay(1500);

}
TestVehicleSystem::~TestVehicleSystem() {
}

