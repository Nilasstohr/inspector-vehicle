/*
 * DualPPositionControl.cpp
 *
 *  Created on: Jun 27, 2023
 *      Author: robot1
 */

#include "DualPPositionControl.h"

DualPPositionControl::DualPPositionControl(ControlEssentials * essentials,double p):
essentials(essentials),p(p),position(0),output(0)
{}

void DualPPositionControl::update(double referencePosition) {
	essentials->setWLeft(getControlOutput(
			QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left,referencePosition));
	essentials->setWRight(getControlOutput(
				QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right,referencePosition));
	essentials->getMotors()->drive(essentials->getWLeft(), essentials->getWRight());
}

double DualPPositionControl::getControlOutput(QuadratureEncoders::QuadratureEncoderSide side,double referencePosition) {
	position = essentials->getSensors()->encoder(side)
							 ->read<double>(QuadratureEncoderReadTypes::position_linear);
	output =  (referencePosition-position)*p;
	if(output<0)
		essentials->getMotors()->setDir(DrivingDirection::BACKWARD);
	else
		essentials->getMotors()->setDir(DrivingDirection::FORWARD);
	return output;
}

DualPPositionControl::~DualPPositionControl() {



}


