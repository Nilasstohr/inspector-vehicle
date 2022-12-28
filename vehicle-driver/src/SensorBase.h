/*
 * VehicleSide.h
 *
 *  Created on: Dec 28, 2022
 *      Author: robot1
 */

#ifndef SRC_SENSORBASE_H_
#define SRC_SENSORBASE_H_

#include "Side.h"

template <typename T>
class SensorBase {
public:

	SensorBase(T left, T right);
	T getBySide(Side side);

private:
	T sensorLeft;
	T sensorRight;
};


#endif /* SRC_SENSORBASE_H_ */
