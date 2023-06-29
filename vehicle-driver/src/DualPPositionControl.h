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
	void update(double referencePosition);
	virtual ~DualPPositionControl();
private:
	double getControlOutput(QuadratureEncoders::QuadratureEncoderSide side,double referencePosition);
	double getPosition(QuadratureEncoders::QuadratureEncoderSide side);
	ControlEssentials * essentials;
	double p;
	double position;
	double output;
};

#endif /* DUALPPOSITIONCONTROL_H_ */
