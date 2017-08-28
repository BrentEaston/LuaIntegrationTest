/*
 * ContextFrame.cpp
 *
 *  Created on: 15 Aug. 2017
 *      Author: Brent
 */

#include <Scripting/ContextFrame.h>
#include <Scripting/Proxy.h>
#include <iostream>

using namespace std;

// Create an empty Context Frame
ContextFrame::ContextFrame() {
	// cout << "ContextFrame::ContextFrame" << endl;
}

// Create a new Context Frame containing just a Proxy for one Scriptable
ContextFrame::ContextFrame(const Scriptable *thisPtr) {
	// cout << "ContextFrame::ContextFrame" << endl;
	// cout << "ContextFrame::ContextFrame Create frame containing proxy for object at " << thisPtr << endl;
	getProxy(thisPtr);
}

ContextFrame::~ContextFrame() {
	// cout << "ContextFrame::~ContextFrame" << endl;
}

std::unique_ptr<Proxy> const & ContextFrame::getProxy(const Scriptable *scriptable) {
	return getProxy (scriptable->getScriptableType(), (void *) scriptable);
}

std::unique_ptr<Proxy> const & ContextFrame::getProxy(Scriptable::eType type, const void *ptr) {
	if (proxies.count(ptr) == 0) {
		proxies[ptr] = move(proxyFactory.createProxy(type, ptr, this));
		// cout << "ContextFrame::getProxy Created proxy at " << proxies[ptr].get() << " for object at " << ptr << ", type " << type << endl;
	}
	return proxies[ptr];
}
