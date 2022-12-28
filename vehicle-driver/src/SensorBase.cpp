/*
 * VehicleSide.cpp
 *
 *  Created on: Dec 28, 2022
 *      Author: robot1
 */

#include <src/SensorBase.h>

template<typename T>
inline SensorBase<T>::SensorBase(T left, T right) {
	this->sensorLeft = left;
	this->sensorRight = right;
}

template<typename T>
inline T SensorBase<T>::getBySide(Side side) {
	switch(side) {
	  case Side::left:
		return this->sensorLeft;
	  default:
		return this->sensorRight;
	}
}
