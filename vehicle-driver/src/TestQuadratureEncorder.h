/*
 * TestQuadratureEncorder.h
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#ifndef SRC_TESTQUADRATUREENCORDER_H_
#define SRC_TESTQUADRATUREENCORDER_H_

#include "VehicleTestBase.h"
#include "QuadratureEncoder.h"

class TestQuadratureEncorder: private VehicleTestBase{
public:
	TestQuadratureEncorder();

	void canSetupQuadureEncoder();

	void canGetIntevalCount();

	void canDetroyQuadureEncoder();

	void canSetupQuadratureEncoders();

	virtual ~TestQuadratureEncorder();

private:
	QuadratureEncoder * quadratureEncoder;
	QuadratureEncoders * quadratureEncoders;
};

#endif /* SRC_TESTQUADRATUREENCORDER_H_ */
