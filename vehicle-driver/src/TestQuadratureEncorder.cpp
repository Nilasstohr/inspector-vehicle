/*
 * TestQuadratureEncorder.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#include "TestQuadratureEncorder.h"

TestQuadratureEncorder::TestQuadratureEncorder()
{
	canSetupQuadureEncoder();
	canGetIntevalCount();
	canDetroyQuadureEncoder();
}

void TestQuadratureEncorder::canSetupQuadureEncoder(){
	Logger::verbose(__FUNCTION__, "");
	this->quadratureEncoder = new QuadratureEncoder(
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_A,
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_B);
}

void TestQuadratureEncorder::canGetIntevalCount() {
	Logger::verbose(__FUNCTION__, "");
	this->quadratureEncoder->getCountInterval();
}

void TestQuadratureEncorder::canDetroyQuadureEncoder() {
	Logger::verbose(__FUNCTION__, "");
	delete this->quadratureEncoder;
}

void TestQuadratureEncorder::canSetupQuadratureEncoders() {
	Logger::verbose(__FUNCTION__, "");
	this->quadratureEncoders = VehicleTestBase::
			VehicleTestToolBox::createQuadratureEncoders();

}

TestQuadratureEncorder::~TestQuadratureEncorder() {

}
