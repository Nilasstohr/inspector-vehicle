/*
 * Side.h
 *
 *  Created on: Dec 28, 2022
 *      Author: robot1
 */

#ifndef SRC_SIDE_H_
#define SRC_SIDE_H_

#include "Logger.h"

class Side {
public:
	enum s{
		left,
		right
	};
	Side(s side);
	operator s() const {
		return side;
	}
	String toString();
private:
	s side;
};

#endif /* SRC_SIDE_H_ */
