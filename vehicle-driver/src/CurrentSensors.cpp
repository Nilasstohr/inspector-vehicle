/*
 * CurrentSensors.cpp
 *
 *  Created on: Dec 28, 2022
 *      Author: robot1
 */

#include "CurrentSensors.h"

template<typename T>
inline CurrentSensors<T>::CurrentSensors(CurrentSensor *left,
		CurrentSensor *right):SensorBase<CurrentSensor>(left, right) {
}

template<typename T>
inline void CurrentSensors<T>::test() {
	CurrentSensor * cs =
			SensorBase<T>::getBySide(Side::left);
}

