/*
 * DualPPositionControl.cpp
 *
 *  Created on: Jun 27, 2023
 *      Author: robot1
 */

#include "DualPPositionControl.h"

DualPPositionControl::DualPPositionControl(ControlEssentials * essentials,double p):
essentials(essentials),p(p),position(0),output(0),minimumOutput(0)
{}

void DualPPositionControl::update(double referencePosition) {

	essentials->setWLeft(getControlOutput(
			QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left,referencePosition));
	essentials->setWRight(getControlOutput(
				QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right,referencePosition));
	//Serial.print(" ");
	///Serial.print(essentials->getWLeft());
	//Serial.print(" ");
	//Serial.println(essentials->getWRight());

	essentials->getMotors()->drive(essentials->getWLeft(), essentials->getWRight());
}

void DualPPositionControl::reset() {
	essentials->reset();
}

double DualPPositionControl::getControlOutput(QuadratureEncoders::QuadratureEncoderSide side,double referencePosition) {
	position = essentials->getSensors()->encoder(side)
							 ->read<double>(QuadratureEncoderReadTypes::position_linear);
	//Serial.print(" count=");
	//Serial.println(essentials->getSensors()->encoder(side)
	//		 ->read<double>(QuadratureEncoderReadTypes::encoder_counts));
	Serial.print(" position=");
	Serial.print(position);
	output =  (referencePosition-position)*p;
	if(output<0)
		essentials->getMotors()->setDir(DrivingDirection::BACKWARD);
	else
		essentials->getMotors()->setDir(DrivingDirection::FORWARD);
	output =  sqrt(pow(output,2));

	output = essentials->checkForSaturation(output,minimumOutput);
	Serial.print(" output=");
	Serial.println(output);
	return output;
}

DualPPositionControl::~DualPPositionControl() {




}


