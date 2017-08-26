/*
 * ProxyPiece.cpp
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#include <Map.h>
#include <Piece.h>
#include <Scripting/ProxyDefinition.h>
#include <Scripting/ProxyOperation.h>
#include <Scripting/ProxyPiece.h>
#include <Scripting/Scriptable.h>
#include <Scripting/ScriptResult.h>
#include <iostream>

#define PROXY_NAME "Piece"
#define PROXY_TYPE Scriptable::eType_Piece

using namespace std;

ProxyPiece::ProxyPiece(Piece *piece) {
	// cout << "ProxyPiece::ProxyPiece" << endl;
	registerProxyInfo();
	this->vassalPiece = piece;
}

ProxyPiece::ProxyPiece(const void *piece) {
	// cout << "ProxyPiece::ProxyPiece" << endl;
	registerProxyInfo();
	setPiece((Piece *)piece);
}

ProxyPiece::~ProxyPiece() {
	// cout << "ProxyPiece::~ProxyPiece" << endl;
}

void ProxyPiece::registerProxyInfo() {
	if (!isRegistered(PROXY_NAME)) {
		auto def = make_unique<ProxyDefinition> (PROXY_NAME, PROXY_TYPE);
		// piece.getName(). Return piece name. No arguments.
		def -> registerOperation("getName");
		// piece.get(name). Return the value of property name. Argument 1 name of property.
		def -> registerOperation("get")->addArgument(TValue::eType_string);
		// piece.getMap(). Return the Map this piece is currently place on. No Arguments. May return nil.
		def -> registerOperation("getMap");
		registerProxy(def);
	}
}

void ProxyPiece::setPiece(Piece *piece) {
	// cout << "ProxyPiece::setPiece piece set to " << piece << endl;
	// cout << "ProxyPiece::setPiece piece name= " << piece->getName() << endl;
	this->vassalPiece = piece;
}

Piece *ProxyPiece::getPiece() {
	return this -> vassalPiece;
}

void ProxyPiece::performOperation(const string operation, vector<unique_ptr<TValue>> &args, ScriptResult &result) {

	// TODO Can this be done as a dispatch table using a map of function pointers?

	// Check that the number and types of arguments are as expected for this operation
	checkOperationArguments (PROXY_NAME, operation, args, result);
	if (! result.isSuccessful()) {
		return;
	}

	if (operation == "getName") {
		result.setResultValue(make_unique<TValue> (getPiece()->getName()));
		//result = make_unique<TValue>(getPiece()->getName());
		return;
	} else if (operation == "get") {
		//result = make_unique<TValue>(getPiece()->get(args[0]->getValueAsString()));
		result.setResultValue(make_unique<TValue> (getPiece()->get(args[0]->getValueAsString())));
		return ;
	} else if (operation == "getMap") {
		// // cout << "In ProxyPiece::performOperation - getMap, piece=" << getPiece() << endl;
		const Map *map = getPiece()->getMap();
		// // cout << "map=" << map << endl;
		//result = make_unique<TValue>((void *) map, map->getScriptableType(), TValue::eType_Vobject);
		result.setResultValue (make_unique<TValue> ((void *) map, map->getScriptableType()));
		return ;
	}
	result.setVassalError("ProxyMap: Unknown operation " + operation);
	return;
}
