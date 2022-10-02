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
	testCanAngularVelocity();
	testCanDoEncoderRecord();
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

void TestVehicleSystem::testCanDoEncoderRecord() {
	Logger::verbose(__FUNCTION__, "- TEST");
	this->resetTest();
	uint32_t tic;
	uint32_t toc;

	tic = micros();
	EncoderRecord * rec = new EncoderRecord(this->encoders()->right());
	toc = micros();

	String * recStr = new String();
	rec->getRecord(recStr);
	recStr->space().append(toc-tic);
	Logger::verbose(recStr->c_str());
	motors()->stop();

}

void TestVehicleSystem::testCanSampleEncoderRecords() {
	Logger::verbose(__FUNCTION__, "- TEST");
	this->record.push_back(EncoderRecord(this->encoders()->left()));
	EncoderRecord a = this->record[0];
	String * recStr = new String();
	a.getRecord(recStr);;
	Logger::verbose(__FUNCTION__,recStr->c_str());

	//sampleTimer->begin(IrqSampleTimer,(unsigned int)(1666));
}


// helpers
String *TestVehicleSystem::count(signed int &count,int side) {
	String * log = encoders()->count(count,(QuadratureEncoders::QuadratureEncoderSide)(side));
	Logger::verbose(__FUNCTION__,log->c_str());
	return log;
}

void TestVehicleSystem::sampleEventTimerHandler() {
	//Logger::verbose(__FUNCTION__, "event");
}

void TestVehicleSystem::resetTest() {
	delay(2000);
	encoders()->reset();
	motors()->forward(65000);
	delay(1500);

}
TestVehicleSystem::~TestVehicleSystem() {
}

