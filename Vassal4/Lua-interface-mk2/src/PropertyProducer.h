/*
 * PropertyProducer.h
 *
 *  Created on: 01/08/2017
 *      Author: Brent
 */

#ifndef PROPERTYPRODUCER_H_
#define PROPERTYPRODUCER_H_

#include <string>

/**
 * A PropertyProducer is inherited by a class that has properties to reveal via a getProperty Call
 */
class PropertyProducer {
public:
	virtual ~PropertyProducer() {};
	virtual std::string getProperty(const std::string propertyName) const=0;
};

#endif /* PROPERTYPRODUCER_H_ */
