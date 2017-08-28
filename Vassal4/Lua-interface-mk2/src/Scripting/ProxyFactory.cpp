/*
 * ProxyFactory.cpp
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#include <assert.h>
#include <Scripting/ProxyCollection.h>
#include <Scripting/ProxyFactory.h>
#include <Scripting/ProxyMap.h>
#include <Scripting/ProxyPiece.h>
#include <iostream>

using namespace std;

ProxyFactory::ProxyFactory() {

}

ProxyFactory::~ProxyFactory() {

}

std::unique_ptr<Proxy> ProxyFactory::createProxy(const Scriptable::eType type, const void *ptr, ContextFrame *frame) {
	// Piece *p;

	switch (type) {
	case Scriptable::eType_Piece:
		//	cout << " ProxyFactory::createProxy:piece, ptr =" << ptr << endl;
		//	p = (Piece *) ptr;
		// cout << " ProxyFactory::createProxy:piece, piece name =" << p->getName() << endl;
		return std::make_unique<ProxyPiece> (ptr, frame);
		break;
	case Scriptable::eType_Map:
		return std::make_unique<ProxyMap> (ptr, frame);
		break;
	case Scriptable::eType_Collection:
		// cout << " ProxyFactory::createProxy:collection, ptr =" << ptr << endl;
		return std::make_unique<ProxyCollection> (ptr, frame);
		break;
	default:
		assert(0);
		break;
	}
	return nullptr;
}

std::unique_ptr<Proxy> ProxyFactory::createProxy (Scriptable *scriptable, ContextFrame *frame) {

	return createProxy (scriptable->getScriptableType(), scriptable, frame);

}


