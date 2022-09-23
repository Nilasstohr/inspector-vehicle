/*
 * QuadratureEncoderInterval.h
 *
 *  Created on: Sep 18, 2022
 *      Author: robot1
 */

#ifndef QUADRATUREENCODERINTERVAL_H_
#define QUADRATUREENCODERINTERVAL_H_

#include <Arduino.h>

class QuadratureEncoderInterval {
public:

	QuadratureEncoderInterval();

	void update(uint64_t value);

	uint16_t get();

	void reset();

	virtual ~QuadratureEncoderInterval();

private:
	uint16_t interval;
	uint64_t last =0;

	uint64_t getLast() const {
		return last;
	}
	void setLast(uint64_t last) {
		this->last = last;
	}
	void setInterval(uint16_t last) {
		this->interval = interval;
	}
};

#endif /* QUADRATUREENCODERINTERVAL_H_ */
