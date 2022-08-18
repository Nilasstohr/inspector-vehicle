/*
 * TestQuadratureEncorder.h
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#ifndef SRC_TESTQUADRATUREENCORDER_H_
#define SRC_TESTQUADRATUREENCORDER_H_

#include "Logger.h"
#include "VehiclePins.h"
#include "QuadratureEncoder.h"

class TestQuadratureEncorder {
public:
	TestQuadratureEncorder();

	void canSetupQuadureEncoder();

	virtual ~TestQuadratureEncorder();


private:
	QuadratureEncoder * quadratureEncoder;
    QuadratureEncoder* getQuadratureEncoder() {
		return quadratureEncoder;
	}
};

#endif /* SRC_TESTQUADRATUREENCORDER_H_ */
