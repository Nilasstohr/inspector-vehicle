/*
 * Side.cpp
 *
 *  Created on: Dec 28, 2022
 *      Author: robot1
 */

#include "Side.h"

Side::Side(s side):side(side) {
}

String Side::toString() {
	if(this->side==left){
		return "left";
	}else{
		return "right";
	}
}
