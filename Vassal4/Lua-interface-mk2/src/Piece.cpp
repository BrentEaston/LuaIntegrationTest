/*
 * Piece.cpp
 *
 *  Created on: 15/07/2017
 *      Author: Brent
 */

#include <Piece.h>
#include <iostream>

using namespace std;

Piece::Piece(const std::string name, const int level) {
	setName(name);
	setLevel(level);
	// cout << "Piece " << name << " Created" << endl;
}

Piece::Piece(Piece *piece) {
	setName (piece->getName());
	setLevel (piece->getLevel());
	// cout << "Piece " << myName << " Created" << endl;
}

Piece::~Piece() {
	// cout << "Piece " << myName << " Destroyed" << endl;
	setName("");
	setLevel(0);
}

int Piece::getLevel() const {
	return myLevel;
}

void Piece::setLevel(const int myLevel) {
	this->myLevel = myLevel;
}

std::string Piece::getName() const {
	return myName + " " + std::to_string(getLevel());
}

std::string Piece::toString() const {
	return getName();
}

void Piece::setName(const std::string &myName) {
	this->myName = myName;
}

Scriptable::eType Piece::getScriptableType() const {
	return Scriptable::eType_Piece;
}

std::string Piece::get(const std::string propertyName) const {

	if (propertyName == "Level") {
		return  std::to_string(getLevel());
	}
	else if (propertyName == "Name") {
		return getName();
	}
	else if (propertyName == "visible") {
		return "true";
	}
	return "v_undefined";
}

void Piece::set (const string propertyName, TValue value) {
	cout << "Piece " << myName << ": Set property " << propertyName << " to value " << value.toString() << endl;
}

void Piece::setMap (Map *map) {
	// cout << "Setting Map to " << map << endl;
	myMap = map;
}

const Map *Piece::getMap() {
	return myMap;
}

