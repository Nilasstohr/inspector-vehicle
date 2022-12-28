/*
 * CurrentSensors.h
 *
 *  Created on: Dec 28, 2022
 *      Author: robot1
 */

#ifndef SRC_CURRENTSENSORS_H_
#define SRC_CURRENTSENSORS_H_

#include "CurrentSensor.h"
#include "SensorBase.h"
#include "Side.h"

template <typename T>
class CurrentSensors: public SensorBase<T> {
public:
	CurrentSensors(CurrentSensor *left,CurrentSensor *right);
	void test();
};


#endif /* SRC_CURRENTSENSORS_H_ */
