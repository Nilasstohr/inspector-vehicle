/*
 * averageCalc.h
 *
 *  Created on: Apr 7, 2023
 *      Author: robot1
 */

#ifndef SRC_AVERAGECALC_H_
#define SRC_AVERAGECALC_H_

#include "Logger.h"

class AverageCalc {
public:
	AverageCalc(int sampleSize);
	bool add(uint32_t interval);
	uint32_t average();
private:
	uint32_t sum;
	int count;
	int sampleSize;
};

#endif /* SRC_AVERAGECALC_H_ */
