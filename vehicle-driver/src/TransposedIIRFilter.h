/*
 * PiController.h
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#ifndef SRC_TRANSPOSEDIIRFILTER_H_
#define SRC_TRANSPOSEDIIRFILTER_H_

class TransposedIIRFilter {
public:
	TransposedIIRFilter(float b0,float b1,float a1);
	double update(double input);
	void reset();
	double get();
	virtual ~TransposedIIRFilter();
private:
	float b0;
	float b1;
	float a1;
	double feedForward;
	double feedBack;
	double error;
	double output;
};

#endif /* SRC_TRANSPOSEDIIRFILTER_H_ */
