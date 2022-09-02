/*
 * Experimentation.h
 *
 *  Created on: Aug 28, 2022
 *      Author: robot1
 */

#ifndef SRC_EXPERIMENTATION_H_
#define SRC_EXPERIMENTATION_H_

#include <src/Logger.h>
#include "QuadratureEncoder.h"

class Experimentation {
public:
	Experimentation();
	virtual ~Experimentation();
private:
	QuadratureEncoder * quadratureEncoderM2;
	QuadratureEncoder * quadratureEncoderM1;
};

#endif /* SRC_EXPERIMENTATION_H_ */
