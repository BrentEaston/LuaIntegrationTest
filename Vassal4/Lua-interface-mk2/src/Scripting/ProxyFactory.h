/*
 * ProxyFactory.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef PROXYFACTORY_H_
#define PROXYFACTORY_H_

#include <Scripting/Scriptable.h>
#include <memory>

class ContextFrame;

class Proxy;

class ProxyFactory {
public:
	ProxyFactory();
	virtual ~ProxyFactory();


	// Create a proxy for a Vassal supplied Scriptable
	std::unique_ptr<Proxy> createProxy (Scriptable *scriptable, ContextFrame *frame);

	// Create a proxy for a raw pointer and type supplied by the scripting environment
	std::unique_ptr<Proxy> createProxy(const Scriptable::eType type, const void *ptr, ContextFrame *frame);
};

#endif /* PROXYFACTORY_H_ */
