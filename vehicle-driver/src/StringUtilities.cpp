/*
 * StringUtilities.cpp
 *
 *  Created on: Sep 23, 2022
 *      Author: robot1
 */

#include "StringUtilities.h"

StringUtilities::StringUtilities() {
	// TODO Auto-generated constructor stub

}

String* StringUtilities::appendDouble(String &string, double value) {
	char buffer[7];
	dtostrf(value,1,6,buffer);
	string.append(buffer);
	return &string;
}

StringUtilities::~StringUtilities() {
	// TODO Auto-generated destructor stub
}

