/*
 * Map.cpp
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#include <Collection.h>
#include <Map.h>
#include <Piece.h>
#include <Scripting/Script.h>
#include <Scripting/ScriptingEnvironment.h>
#include <Scripting/ScriptingFactory.h>
#include <Scripting/ScriptResult.h>
#include <iostream>

using namespace std;

Map::Map(const std::string name) {
	setName(name);
}

Map::Map(Map *map) {
	setName(map->getName());
}

Map::~Map() {
	// TODO Auto-generated destructor stub
}

std::string Map::getName() const {
	return myName;
}

void Map::setName(const std::string &myName) {
	this->myName = myName;
}

Scriptable::eType Map::getScriptableType() const {
	return Scriptable::eType_Map;
}

TValue *Map::get(const std::string propertyName) const {
	// cout << "   in Map::get looking up property name " << propertyName << " = " << getName() << endl;
	if (propertyName == "Name") {
		return new TValue(getName());
	} else if (propertyName == "runScript") {
		string s = "local x = 10\n"
				"local y = 20\n"
				"print('Some output from nested script')\n"
				"setmetatable(_ENV, {})";
		ScriptingEnvironment *environment = ScriptingFactory::getInstance() -> getEnvironment().get();
		Script script (environment, s, "MapTestLower");
		ScriptResult result;
		script.execute(this, result);
	}
	return new TValue("");
}

void Map::set (const string propertyName, TValue value) {
	cout << "Map " << myName << ": Set property " << propertyName << " to value " << value.toString() << endl;
}

TValue *Map::properties() const {
	return new TValue();
}


Collection *Map::getVisiblePieces() {

	//std::vector<Piece> *pieces = new std::vector<Piece>();
	Collection *collection = new Collection();

	collection->addItem(new Piece("Piece1", 1));
	// cout << "Map::getVisiblePieces Piece 1 is at " << collection->getItem(0) << endl;
	collection->addItem(new Piece("Piece2", 2));
	//	cout << "Map::getVisiblePieces Piece 2 is at " << & visiblePieces->at(1) << endl;
	collection->addItem(new Piece("Piece3", 3));
	//	cout << "Map::getVisiblePieces Piece 3 is at " << & visiblePieces->at(2) << endl;

	visiblePieces = unique_ptr<Collection> (collection);

	return visiblePieces.get();
}
