/*
 * PiController.h
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#ifndef SRC_FIRSTORDERFILTER_H_
#define SRC_FIRSTORDERFILTER_H_

class FirstOrderFilter {
public:
	FirstOrderFilter(float b0,float b1,float a1);
	double update(double input,double ref);
	virtual ~FirstOrderFilter();
	float b0;
	float b1;
	float a1;
	double feedForward;
	double feedBack;
	double error;
	double output;
};

#endif /* SRC_FIRSTORDERFILTER_H_ */
