/*
 * ProxyMap.cpp
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#include <Map.h>
#include <Scripting/ProxyDefinition.h>
#include <Scripting/ProxyMap.h>
#include <Scripting/ProxyOperation.h>
#include <Scripting/Scriptable.h>
#include <Scripting/ScriptResult.h>
#include <algorithm>
#include <iostream>
#include <iterator>

#define PROXY_NAME "Map"
#define PROXY_TYPE Scriptable::eType_Map

using namespace std;

ProxyMap::ProxyMap(Map *vassalMap) {
	// cout << "ProxyMap::ProxyMap" << endl;
	registerProxyInfo();
	this -> vassalMap = vassalMap;
}

ProxyMap::~ProxyMap() {
	// cout << "ProxyMap::~ProxyMap" << endl;
}

ProxyMap::ProxyMap(const void *map) {
	// cout << "ProxyMap::ProxyMap" << endl;
	registerProxyInfo();
	setMap((Map *) map);
}

void ProxyMap::registerProxyInfo() {
	if (!isRegistered(PROXY_NAME)) {
		auto def = make_unique<ProxyDefinition> (PROXY_NAME, PROXY_TYPE);
		def -> registerOperation("getName");
		def -> registerOperation("get")->addArgument(TValue::eType_string);
		def -> registerOperation("getVisiblePieces");
		def -> registerOperation("getVisiblePiecesNext");
		registerProxy(def);
	}
}

void ProxyMap::setMap (Map *vassalMap) {
	this->vassalMap = vassalMap;
}

Map *ProxyMap::getMap() {
	return vassalMap;
}


void ProxyMap::performOperation(const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result) {

// cout << "ProxyMap::performOperation: operation=" << operation << endl;
	// Check that the number and types of arguments are as expected for this operation
	checkOperationArguments (PROXY_NAME, operation, args, result);
	if (! result.isSuccessful()) {
		return;
	}


	if (operation == "getName") {
		//result = make_unique<TValue>(getMap()->getName());
		result.setResultValue(make_unique<TValue>(getMap()->getName()));
		return;

	} else if (operation == "get") {
		//result = make_unique<TValue>(getMap()->get(args[0]->getValueAsString()));
		result.setResultValue (make_unique<TValue> (getMap()->get(args[0]->getValueAsString())));
		return ;

	} else if (operation == "getVisiblePieces") {

		visiblePieces = std::move(vassalMap->getVisiblePieces());
		visiblePiecesIterator = visiblePieces->begin();

		// cout << "ProxyMap::performOperation:getVisiblePieces Piece 1 is at " << & (visiblePieces->at(0)) << endl;
		// cout << "ProxyMap::performOperation:getVisiblePieces Piece 1 name is " << visiblePieces->at(0).getName() << endl;

		result.setResultValue (make_unique<TValue> ((void *) vassalMap, vassalMap->getScriptableType()));
		return;

	} else if (operation == "getVisiblePiecesNext") {

		if (visiblePiecesIterator == visiblePieces->end()) {
			result.setResultValue(make_unique<TValue>());
		}
		else {
			Piece *p = &(*visiblePiecesIterator);
			// cout << "ProxyMap::performOperation:getVisiblePiecesNext p=" << p << endl;
			// cout << "ProxyMap::performOperation:getVisiblePiecesNext *p.getName*()=" << (*p).getName() << endl;
			result.setResultValue (make_unique<TValue>((void *) p, Scriptable::eType_Piece));
			visiblePiecesIterator++;
		}
		return;
	}

	result.setVassalError("ProxyMap: Unknown operation " + operation);
	return ;
}

