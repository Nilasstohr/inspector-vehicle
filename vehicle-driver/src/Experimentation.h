/*
 * Experimentation.h
 *
 *  Created on: Aug 28, 2022
 *      Author: robot1
 */

#ifndef SRC_EXPERIMENTATION_H_
#define SRC_EXPERIMENTATION_H_

#include <src/Logger.h>
#include "QuadratureEncoders.h"

class Experimentation {
public:
	Experimentation();
	virtual ~Experimentation();
private:
	QuadratureEncoders * quadratureEncoders;
};

#endif /* SRC_EXPERIMENTATION_H_ */
