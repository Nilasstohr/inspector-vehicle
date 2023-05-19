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

	void update(uint32_t value);

	uint32_t get();

	void reset();

	virtual ~QuadratureEncoderInterval();

private:
	uint32_t interval;
	uint32_t last =0;

	uint32_t getLast() const {
		return last;
	}
	void setLast(uint32_t last) {
		this->last = last;
	}
	void setInterval(uint32_t interval) {
		this->interval = interval;
	}
};

#endif /* QUADRATUREENCODERINTERVAL_H_ */
