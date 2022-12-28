/*
 * EncoderRecord.h
 *
 *  Created on: Oct 2, 2022
 *      Author: robot1
 */

#ifndef SRC_ENCODERRECORD_H_
#define SRC_ENCODERRECORD_H_

#include <map>
#include "QuadratureEncoderTypes.h"
#include "QuadratureEncoder.h"

class EncoderRecord {
public:
	EncoderRecord(QuadratureEncoder * encoder,std::map<QuadratureEncoderReadTypes,bool> *filter);
	EncoderRecord(QuadratureEncoder * encoder);

	void getRecord(String * record);
	double getRadianPrSecond();

	virtual ~EncoderRecord();

private:
	 std::map<QuadratureEncoderReadTypes,bool> *filter;
	 std::map<QuadratureEncoderReadTypes, bool> * getFilter() {
		return filter;
	 }
	 QuadratureEncoder * encoder;
	 QuadratureEncoder * getEncoder() const {
	 		return encoder;
	 	}
	 bool isFilterSet();
	 void record(QuadratureEncoder * encoder);
	 bool isUnFiltered(QuadratureEncoderReadTypes type);

	 // record variables
	 signed int count;
	 uint32_t timeIntervalMiros;
	 double radiandPrSecond;
	 void setCount();
	 void setIntervalMiros();
	 void setRadiandPrSecond(QuadratureEncoderReadTypes type);
	 // getter
	 uint32_t getTimeIntervalMiros() const {
		return timeIntervalMiros;
	}
};

#endif /* SRC_ENCODERRECORD_H_ */
