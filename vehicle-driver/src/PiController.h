/*
 * PiController.h
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#ifndef SRC_PICONTROLLER_H_
#define SRC_PICONTROLLER_H_

class PiController {
public:
	PiController(float b0,float b1,float a1);
	double update(double input,double ref);
	virtual ~PiController();
	float b0;
	float b1;
	float a1;
	double feedForward;
	double feedBack;
	double error;
	double output;
};

#endif /* SRC_PICONTROLLER_H_ */
