/*
 * QuadratureEncoderInterval.cpp
 *
 *  Created on: Sep 18, 2022
 *      Author: robot1
 */

#include "QuadratureEncoderInterval.h"

QuadratureEncoderInterval::QuadratureEncoderInterval() {
	this->interval = 0;
}

uint16_t QuadratureEncoderInterval::get() {
	return this->interval;
}

void QuadratureEncoderInterval::update(uint64_t value) {
	this->setInterval(value - this->getLast());
	this->setLast(value);
}

void QuadratureEncoderInterval::reset() {
	this->setInterval(0);
	this->setLast(0);
}

QuadratureEncoderInterval::~QuadratureEncoderInterval() {
	// TODO Auto-generated destructor stub
}

