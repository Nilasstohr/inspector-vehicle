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
	double get() const;

	/**
	 * Anti-windup back-calculation: force the filter's internal integrator
	 * state so that the *next* call to update() starts accumulating from
	 * clampedOutput rather than from whatever the unclamped output was.
	 *
	 * Call this immediately after clamping the output of a PI controller:
	 *   double u = filter.update(error);
	 *   if (u > uMax) { u = uMax; filter.resetToOutput(uMax); }
	 *
	 * Derivation (transposed direct-form II, a1 = -1 for pure integrator):
	 *   feedBack[n] = a1 * output[n]
	 * Overwriting feedBack with a1*clampedOutput means the next update
	 * computes  output = b0*e + feedForward - a1*clampedOutput  correctly.
	 */
	void resetToOutput(double clampedOutput);

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
