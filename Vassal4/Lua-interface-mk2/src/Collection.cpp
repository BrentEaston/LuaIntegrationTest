/*
 * Collection.cpp
 *
 *  Created on: 27 Aug. 2017
 *      Author: Brent
 */

#include <Collection.h>
#include <Scripting/TValue.h>
#include <iostream>
#include <string>

using namespace std;

Collection::Collection() {
	// TODO Auto-generated constructor stub

}

Collection::~Collection() {
	cout << "Warning: Collection destroyed" << endl;
}

void Collection::addItem(Scriptable *item) {
	contents.push_back(item);
}

Scriptable *Collection::getItem(int position) const {
	return contents[position];
}

int Collection::size() const {
	return contents.size();
}

Scriptable::eType Collection::getScriptableType() const {
	return Scriptable::eType_Collection;
}

TValue *Collection::get(const std::string propertyName) const {
	cout << "   in Collection::get looking up property name " << propertyName << endl;

	return new TValue("");
}

void Collection::set (const string propertyName, TValue value) {
	cout << "Collection: Set property " << propertyName << " to value " << value.toString() << endl;
}

TValue *Collection::properties() const {
	return new TValue();
}
