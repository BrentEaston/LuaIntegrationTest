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

		// piece.set(name, value). Set the property name to value
		def -> registerOperation("set")->addArgument(TValue::eType_string)->addArgument(TValue::eType_any);

		// piece.getMap(). Return the Map this piece is currently place on. No Arguments. May return nil.
		def -> registerOperation("getMap");

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

	// cout << "ProxyPiece::performOperation: operation = [" << operation << "]" << endl;

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
		// FIXME Ugly code, how can we make this look better?
		result.setResultValue (make_unique<TValue> ((void *) map, map->getScriptableType()));
		return ;

	} else if (operation == "set") {

		// TODO
		getPiece()->set(args[0]->getStringValue(), *(args[1].get()));
		result.setResultValue(make_unique<TValue> (""));
		return;
	}
	result.setVassalError("ProxyPiece: Unknown operation " + operation);
	return;
}
