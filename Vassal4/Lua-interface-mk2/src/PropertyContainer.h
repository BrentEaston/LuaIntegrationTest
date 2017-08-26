/*
 * PropertyContainer.h
 *
 *  Created on: 01/08/2017
 *      Author: Brent
 */

#ifndef PROPERTYCONTAINER_H_
#define PROPERTYCONTAINER_H_

#include <string>

using namespace std;

class TValue;

/**
 *
 */
class PropertyContainer {
public:
	virtual ~PropertyContainer() {};

	virtual TValue *get(const string id) const=0;
	virtual void set (const string id, TValue value)=0;
	virtual TValue *properties() const=0;
};

#endif /* PROPERTYCONTAINER_H_ */
