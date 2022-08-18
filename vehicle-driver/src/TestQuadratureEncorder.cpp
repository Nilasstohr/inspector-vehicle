/*
 * TestQuadratureEncorder.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#include "TestQuadratureEncorder.h"

TestQuadratureEncorder::TestQuadratureEncorder()
{
	Logger::verbose(__FUNCTION__,"");
	canSetupQuadureEncoder();
}

void TestQuadratureEncorder::canSetupQuadureEncoder(){
	Logger::verbose(__FUNCTION__, "");
	this->quadratureEncoder = new QuadratureEncoder(
				VEHICLE_PIN_QUADENCODER_MOTOR1_CHANNEL_A,
				VEHICLE_PIN_QUADENCODER_MOTOR1_CHANNEL_B);

}

TestQuadratureEncorder::~TestQuadratureEncorder() {
	// TODO Auto-generated destructor stub
}
