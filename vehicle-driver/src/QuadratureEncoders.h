/*
 * QuadratureEncoders.h
 *
 *  Created on: Sep 4, 2022
 *      Author: robot1
 */

#ifndef SRC_QUADRATUREENCODERS_H_
#define SRC_QUADRATUREENCODERS_H_

#include "QuadratureEncoder.h"

class QuadratureEncoders {
public:
	QuadratureEncoders(int channel1A,int channel1B,int channel2A,int channel2B);
	void setupEncoders();
	virtual ~QuadratureEncoders();
	QuadratureEncoder * getQuadratureEncoderLeft();
	QuadratureEncoder * getQuadratureEncoderRight();
private:
	QuadratureEncoder * quadratureEncoderLeft;
	QuadratureEncoder * quadratureEncoderRight;

};

#endif /* SRC_QUADRATUREENCODERS_H_ */
