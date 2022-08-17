/*
 * TestQuadratureEncorder.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: robot1
 */

#include "TestQuadratureEncorder.h"
#include "src/QuadratureEncoder.h"



TestQuadratureEncorder::TestQuadratureEncorder(int pinChannelA,int pinChannelB)
:pinChannelA(pinChannelA),
 pinChannelB(pinChannelB)
{
	// TODO Auto-generated constructor stub
}

void TestQuadratureEncorder::canSetupQuadureEncoder(){
	QuadratureEncoder * quadratureEncoder = new QuadratureEncoder(
			this->getPinChannelA(),this->getPinChannelB());

}

TestQuadratureEncorder::~TestQuadratureEncorder() {
	// TODO Auto-generated destructor stub
}

