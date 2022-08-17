/*
 * TestQuadratureEncorder.h
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#ifndef SRC_TESTQUADRATUREENCORDER_H_
#define SRC_TESTQUADRATUREENCORDER_H_

class TestQuadratureEncorder {
public:
	TestQuadratureEncorder(int pinChannelA,int pinChannelB);

	void canSetupQuadureEncoder();

	virtual ~TestQuadratureEncorder();

	int getPinChannelA() const {
		return pinChannelA;
	}

	void setPinChannelA(int pinChannelA) {
		this->pinChannelA = pinChannelA;
	}

	int getPinChannelB() const {
		return pinChannelB;
	}

	void setPinChannelB(int pinChannelB) {
		this->pinChannelB = pinChannelB;
	}

private:
	int pinChannelA;
	int pinChannelB;
};

#endif /* SRC_TESTQUADRATUREENCORDER_H_ */
