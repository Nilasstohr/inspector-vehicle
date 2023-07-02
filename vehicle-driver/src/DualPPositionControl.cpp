/*
 * DualPPositionControl.cpp
 *
 *  Created on: Jun 27, 2023
 *      Author: robot1
 */

#include "DualPPositionControl.h"

DualPPositionControl::DualPPositionControl(ControlEssentials * essentials,double p):
essentials(essentials),p(p),position(0){
	reset();
}

void DualPPositionControl::update() {

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
	position=0;
	output=0;
	minimumOutput=0;
	referencePosition=0;
	referenceTolerance=0;
}

double DualPPositionControl::getLeftPosition() {
	return getPosition(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left);
}

double DualPPositionControl::getRightPosition() {
	return getPosition(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right);
}

void DualPPositionControl::setReferencePosition(double referencePosition) {
	this->referencePosition = referencePosition;
}

void DualPPositionControl::setReferenceTolerance(double referenceTolerance) {
	this->referenceTolerance = referenceTolerance;
}

bool DualPPositionControl::isPositionReached() {
	return isPositionWithInTolerances(positionLeft) && isPositionWithInTolerances(positionRight);
}

bool DualPPositionControl::isPositionWithInTolerances(double position) {
	return position > referencePosition -referenceTolerance &&
		   position < referencePosition + referenceTolerance;
}


double DualPPositionControl::getPosition(
		QuadratureEncoders::QuadratureEncoderSide side) {
	return essentials->getSensors()->encoder(side)
			 ->read<double>(QuadratureEncoderReadTypes::position_linear);
}

signed int DualPPositionControl::getCount(
		QuadratureEncoders::QuadratureEncoderSide side) {
	return essentials->getSensors()->encoder(side)
			 ->read<signed int>(QuadratureEncoderReadTypes::position_linear);
}


void DualPPositionControl::setPosition(
		QuadratureEncoders::QuadratureEncoderSide side, double position) {
	switch(side){
		case QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left:
			positionLeft = position;
			break;
		case QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right:
			positionRight = position;
			break;
	}
}


double DualPPositionControl::getControlOutput(QuadratureEncoders::QuadratureEncoderSide side,double referencePosition) {
	position = getPosition(side);
	setPosition(side, position);
	//Serial.print(" count=");
	//Serial.println(essentials->getSensors()->encoder(side)
	//		 ->read<double>(QuadratureEncoderReadTypes::encoder_counts));
	//Serial.print(" position=");
	//Serial.println(position);
	output =  (referencePosition-position)*p;
	if(output<0)
		essentials->getMotors()->setDir(DrivingDirection::BACKWARD);
	else
		essentials->getMotors()->setDir(DrivingDirection::FORWARD);
	output =  sqrt(pow(output,2));

	output = essentials->checkForSaturation(output,minimumOutput);
	//Serial.print(" output=");
	//Serial.println(output);
	return output;
}



DualPPositionControl::~DualPPositionControl() {
}

