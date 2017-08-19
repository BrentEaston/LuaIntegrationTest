/*
 * Expression.cpp
 *
 *  Created on: 08/08/2017
 *      Author: Brent
 */

#include <Scripting/Expression.h>

Expression::~Expression() {
	// TODO Auto-generated destructor stub
}

/**
 * If the source does not commence with 'return', add it in.
 */
std::string Expression::getExecutableSource() const {
	if (getSource().find("return") == 0) {
		return source;
	}
	else {
		return "return " + source;
	}
}

