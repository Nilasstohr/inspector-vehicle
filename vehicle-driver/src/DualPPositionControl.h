/*
 * DualPPositionControl.h
 *
 *  Created on: Jun 27, 2023
 *      Author: robot1
 */

#ifndef DUALPPOSITIONCONTROL_H_
#define DUALPPOSITIONCONTROL_H_

#include "ControlEssentials.h"

class DualPPositionControl {
public:
	DualPPositionControl(ControlEssentials * essentials,double p);
	void update();
	void reset();
	double getLeftPosition();
	double getRightPosition();
	void setReferencePosition(double referencePosition);
	void setReferenceTolerance(double tolerance);
	signed int DualPPositionControl::getCount(
			QuadratureEncoders::QuadratureEncoderSide side);
	bool   isPositionReached();
	virtual ~DualPPositionControl();
private:
	double getControlOutput(QuadratureEncoders::QuadratureEncoderSide side, double referencePosition);
	double getPosition(QuadratureEncoders::QuadratureEncoderSide side);
	void setPosition(QuadratureEncoders::QuadratureEncoderSide side,double position);
	bool isPositionWithInTolerances(double position);
	ControlEssentials * essentials;
	double p;
	double position;
	double output;
	double minimumOutput;
	double referencePosition;
	double referenceTolerance;
	double positionLeft;
	double positionRight;

};

#endif /* DUALPPOSITIONCONTROL_H_ */
