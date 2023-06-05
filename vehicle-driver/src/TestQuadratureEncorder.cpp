/*
 * TestQuadratureEncorder.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#include "TestQuadratureEncorder.h"

TestQuadratureEncorder::TestQuadratureEncorder()
{
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "############# TEST CLASS ############# ");
	canSetEncoderParams();
	canSetupQuadureEncoder();
	canGetIntevalCount();
	canDetroyQuadureEncoder();
}

void TestQuadratureEncorder::canSetEncoderParams() {
	Logger::verbose(__FUNCTION__, " - TEST");
	QuadratureEncorderParameters* params = new QuadratureEncorderParameters(
				VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_A,
				VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_B,
				VEHICLE_WHEEL_RADIUS_CM,
				VEHICLE_MOTOR_ENCODER_COUNT_PR_REV);

	if( params->getPinChannelA()!= VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_A ||
	    params->getPinChannelB()!= VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_B)
	{
		this->printEncoderParmas(params);
	}
	uint64_t count = 10;
	double constantCountsToCm =  (double)((VEHICLE_WHEEL_RADIUS_CM*M_PI)/VEHICLE_MOTOR_ENCODER_COUNT_PR_REV);
	char buffer [100];
	dtostrf(constantCountsToCm,1,6,buffer);

	String *s = new String();

	s->append("\nwheel radius is ").append(VEHICLE_WHEEL_RADIUS_CM).append(" cm");
	s->append("\ncount pr rev ").append(VEHICLE_MOTOR_ENCODER_COUNT_PR_REV);
	s->append("\nomkreds is ").append(VEHICLE_WHEEL_RADIUS_CM*M_PI).append(" cm");
	s->append("\nconv is ").append(buffer).append("\n");
	Logger::verbose(s->c_str());


    double distance = (double)(count*constantCountsToCm);

	if(params->calculateCmFromCount(count)!=distance)
	{
		Serial.print("expected distance to be ");
		Serial.print(distance,6);
		Serial.println(" cm");
		Serial.print("but got ");
		Serial.println(params->calculateCmFromCount(count),6);
		Logger::error("");
	}
}

void TestQuadratureEncorder::printEncoderParmas(QuadratureEncorderParameters *params) {
	char buffer [100];
	sprintf (buffer, "Configuration are: %d,%d",
			params->getPinChannelA(),
			params->getPinChannelB());
	Logger::error(buffer);
	sprintf (buffer, "Expected: %d,%d",
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_A,
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_B);
	Logger::error(buffer);
}


void TestQuadratureEncorder::canSetupQuadureEncoder(){
	Logger::verbose(__FUNCTION__, "TEST");


	TransposedIIRFilter * sensorFilterLeft = new TransposedIIRFilter(
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1);

	this->quadratureEncoder = new QuadratureEncoder(
			new QuadratureEncorderParameters(
				VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_A,
				VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_B,
				VEHICLE_WHEEL_RADIUS_CM,
				VEHICLE_MOTOR_ENCODER_COUNT_PR_REV),sensorFilterLeft);
}

void TestQuadratureEncorder::canGetIntevalCount() {
	Logger::verbose(__FUNCTION__, "TEST");
	this->quadratureEncoder->timeInterval();
}

void TestQuadratureEncorder::canDetroyQuadureEncoder() {
	Logger::verbose(__FUNCTION__, "TEST");
	delete this->quadratureEncoder;
}

void TestQuadratureEncorder::canSetupQuadratureEncoders() {
	Logger::verbose(__FUNCTION__, "TEST");
	this->quadratureEncoders = VehicleTestBase::
			VehicleTestToolBox::createQuadratureEncoders();

}

TestQuadratureEncorder::~TestQuadratureEncorder() {

}
