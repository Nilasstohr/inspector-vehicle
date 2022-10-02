/*
 * EncoderRecord.cpp
 *
 *  Created on: Oct 2, 2022
 *      Author: robot1
 */

#include "EncoderRecord.h"

EncoderRecord::EncoderRecord(QuadratureEncoder * encoder) {
	this->record(encoder);

}

EncoderRecord::EncoderRecord(QuadratureEncoder *encoder,
		std::map<QuadratureEncoderReadTypes, bool> *filter) {
	this->filter = filter;
	this->record(encoder);
}

EncoderRecord::~EncoderRecord() {
}

bool EncoderRecord::isFilterSet() {
	return this->getFilter()!=NULL;
}

void EncoderRecord::record(QuadratureEncoder *encoder) {
	this->encoder = encoder;
	setCount();
	setIntervalMiros();
	this->setRadiandPrSecond(QuadratureEncoderReadTypes::angular_velocity_radian_pr_sec);
	/*
	for(uint8_t i=0; i<QuadratureEncoderReadTypes::size-1;i++){
		QuadratureEncoderReadTypes type = (QuadratureEncoderReadTypes)(i);
		if(type!=QuadratureEncoderReadTypes::encoder_counts &&
		   type!=QuadratureEncoderReadTypes::time_interval_micros){
			switch(type){
				case QuadratureEncoderReadTypes::angular_velocity_radian_pr_sec:{
					this->setRadiandPrSecond(type);
				}
			}
		}

	}
	*/
}

bool EncoderRecord::isUnFiltered(QuadratureEncoderReadTypes type) {
	return true;
}

void EncoderRecord::getRecord(String *record) {
	record->append(this->count).space();
	record->append(this->timeIntervalMiros).space();
	record->append(this->radiandPrSecond).newLine();
}

void EncoderRecord::setCount() {
	this->count =
			this->getEncoder()->read<signed int>(QuadratureEncoderReadTypes::encoder_counts);
}

void EncoderRecord::setIntervalMiros() {
	this->timeIntervalMiros =
			this->getEncoder()->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros);
}

void EncoderRecord::setRadiandPrSecond(QuadratureEncoderReadTypes type) {
	if(this->isUnFiltered(type)){
		this->radiandPrSecond =
				this->getEncoder()->getParameters()->calculateAngularVelocity(this->getTimeIntervalMiros());
	}
}
