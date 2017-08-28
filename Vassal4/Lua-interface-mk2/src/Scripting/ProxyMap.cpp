/*
 * ProxyMap.cpp
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#include <Collection.h>
#include <Map.h>
#include <Scripting/ContextFrame.h>
#include <Scripting/ProxyCollection.h>
#include <Scripting/ProxyDefinition.h>
#include <Scripting/ProxyMap.h>
#include <Scripting/ProxyOperation.h>
#include <Scripting/Scriptable.h>
#include <Scripting/ScriptResult.h>
#include <algorithm>
#include <iostream>

class ProxyFactory;

#define PROXY_NAME "Map"
#define PROXY_TYPE Scriptable::eType_Map

using namespace std;

ProxyMap::ProxyMap(Map *vassalMap, ContextFrame *frame) {
	// cout << "ProxyMap::ProxyMap" << endl;
	registerProxyInfo();
	setMap(vassalMap);
	setFrame (frame);
}

ProxyMap::~ProxyMap() {
	if (! isDestroyed()) {
		vassalMap->removeDestructionListener(this);
	}
}

ProxyMap::ProxyMap(const void *map, ContextFrame *frame) {
	// cout << "ProxyMap::ProxyMap" << endl;
	registerProxyInfo();
	setMap((Map *) map);
	setFrame (frame);
}

void ProxyMap::registerProxyInfo() {
	if (!isRegistered(PROXY_NAME)) {
		auto def = make_unique<ProxyDefinition> (PROXY_NAME, PROXY_TYPE);
		def -> registerOperation("getName");

		def -> registerOperation("get")->addArgument(TValue::eType_string);

		// map.set(name, value). Set the property name to value
		def -> registerOperation("set")->addArgument(TValue::eType_string)->addArgument(TValue::eType_any);

		def -> registerOperation("getVisiblePieces");
		def -> registerOperation("getVisiblePiecesNext");
		registerProxy(def);
	}
}

void ProxyMap::setMap (Map *vassalMap) {
	this->vassalMap = vassalMap;
	vassalMap->addDestructionListener(this);
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

		// Retrieve the collection from the Vassal Map
		Collection *pieces = vassalMap->getVisiblePieces();

		// cout << "getVisiblePiece: Collection created at " << pieces << endl;
		result.setResultValue (make_unique<TValue> ((void *) pieces, pieces->getScriptableType()));

		// cout << "getVisiblePieces: Result set to " << result.getResult() ->toString() << endl;
		return;

	} else if (operation == "getVisiblePiecesNext") {

		//if (visiblePiecesIterator == visiblePieces->end()) {
		//	result.setResultValue(make_unique<TValue>());
		//}
		//else {
		//	Piece *p = &(*visiblePiecesIterator);
			// cout << "ProxyMap::performOperation:getVisiblePiecesNext p=" << p << endl;
			// cout << "ProxyMap::performOperation:getVisiblePiecesNext *p.getName*()=" << (*p).getName() << endl;
		//	result.setResultValue (make_unique<TValue>((void *) p, Scriptable::eType_Piece));
		//	visiblePiecesIterator++;
		//}
		return;
	}

	result.setVassalError("ProxyMap: Unknown operation " + operation);
	return ;
}

