/*
 * ProxyFactory.cpp
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#include <assert.h>
#include <Scripting/ProxyFactory.h>
#include <Scripting/ProxyMap.h>
#include <Scripting/ProxyPiece.h>
#include <iostream>

using namespace std;

ProxyFactory::ProxyFactory() {

}

ProxyFactory::~ProxyFactory() {

}

std::unique_ptr<Proxy> ProxyFactory::createProxy(const Scriptable::eType type, const void *ptr) {
	// Piece *p;

	switch (type) {
	case Scriptable::eType_Piece:
		//	cout << " ProxyFactory::createProxy:piece, ptr =" << ptr << endl;
		//	p = (Piece *) ptr;
		// cout << " ProxyFactory::createProxy:piece, piece name =" << p->getName() << endl;
		return std::make_unique<ProxyPiece> (ptr);
		break;
	case Scriptable::eType_Map:
		return std::make_unique<ProxyMap> (ptr);
		break;
	default:
		assert(0);
		break;
	}
	return nullptr;
}

std::unique_ptr<Proxy> ProxyFactory::createProxy (Scriptable *scriptable) {

	return createProxy (scriptable->getScriptableType(), scriptable);

}


