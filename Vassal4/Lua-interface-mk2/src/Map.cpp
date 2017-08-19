/*
 * Map.cpp
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#include "Map.h"

#include <iostream>
#include <memory>

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

std::string Map::getProperty(const std::string propertyName) const {
	// cout << "   in Map::getProperty looking up property name " << propertyName << " = " << getName() << endl;
	if (propertyName == "Name") {
		return getName();
	}
	return "v_undefined";
}

std::unique_ptr<std::vector<Piece>> Map::getVisiblePieces() {

	//std::vector<Piece> *pieces = new std::vector<Piece>();
	std::unique_ptr<std::vector<Piece>> visiblePieces = std::make_unique<std::vector<Piece>>();

	visiblePieces->push_back(new Piece("Piece1", 1));
	//	cout << "Map::getVisiblePieces Piece 1 is at " << & visiblePieces->at(0) << endl;
	visiblePieces->push_back(new Piece("Piece2", 2));
	//	cout << "Map::getVisiblePieces Piece 2 is at " << & visiblePieces->at(1) << endl;
	visiblePieces->push_back(new Piece("Piece3", 3));
	//	cout << "Map::getVisiblePieces Piece 3 is at " << & visiblePieces->at(2) << endl;

	return visiblePieces;
}
