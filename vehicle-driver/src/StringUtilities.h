/*
 * StringUtilities.h
 *
 *  Created on: Sep 23, 2022
 *      Author: robot1
 */

#ifndef SRC_STRINGUTILITIES_H_
#define SRC_STRINGUTILITIES_H_

#include "Logger.h"

class StringUtilities {
public:
	StringUtilities();

	String * appendDouble(String & string,double value);

	virtual ~StringUtilities();

};

#endif /* SRC_STRINGUTILITIES_H_ */
