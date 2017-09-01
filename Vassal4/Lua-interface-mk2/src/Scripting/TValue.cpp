/*
 * TValue.cpp
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#include <Scripting/TValue.h>
#include <iostream>


using namespace std;

// 	enum eType = {eType_invalid, eType_integer, eType_float, eType_boolean, eType_string, eType_nil, eType_Vobject}
std::string TValue::typeNames[] = {"Invalid", "Any", "Integer", "Float", "Boolean", "String", "Nil", "Object"};

TValue::TValue() {
	setNil();
}

TValue::~TValue() {

}

TValue::TValue(const int i) {
	setInteger(i);
}

TValue::TValue(const float n) {
	setFloat(n);
}

TValue::TValue(const bool b) {
	setBoolean(b);
}
TValue::TValue(const std::string &s) {
	setString(s);
}
TValue::TValue(const char *s) {
	setString(s);
}


TValue::TValue(void *p, const Scriptable::eType objectType) {
	setObject(p, objectType);
}

TValue::TValue(void *p) {
	setPointer(p);
	setType(eType_Vobject);
}

TValue::TValue(Scriptable *scriptable) {
	setPointer(scriptable);
	setType(eType_Vobject);
	setObjectType(scriptable->getScriptableType());
}


const TValue::eType TValue::getType() const{
	return valueType;
}

void TValue::setType(const TValue::eType type) {
	// cout << "TValue::setType setting type to " << type << endl;
	this->valueType = type;
	// cout << "TValue::setType type been set to " << getType() << endl;
}

void TValue::setObjectType(const Scriptable::eType type) {
	this->objectType = type;
}

// Force the conversion of the value to a string
const std::string TValue::getValueAsString() const{
	switch (getType()) {
	case eType_integer:
		return std::to_string(getIntegerValue());
		break;
	case eType_float:
		return std::to_string(getFloatValue());
		break;
	case eType_boolean:
		return getBooleanValue() ? "true" : "false";
		break;
	case eType_string:
		return getStringValue();
		break;
	case eType_Vobject:
		return Scriptable::getTypeName(getObjectType());
		break;
	case eType_nil:
		return "";
		break;
	default:
		break;
	}

	return "Unknown";
}

const int TValue::getIntegerValue() const {
	return value.i;
}

const bool TValue::getBooleanValue() const {
	return value.b;
}

const float TValue::getFloatValue() const {
	return value.n;
}

const std::string TValue::getStringValue() const {
	return stringValue;
}

void * TValue::getObjectPointer() const {
	return value.p;
}

const Scriptable::eType TValue::getObjectType() const {
	return objectType;
}


void TValue::setNil() {
	setType(eType_nil);
}

void TValue::setInteger(const int i) {
	value.i = i;
	setType(eType_integer);
}

void TValue::setBoolean(const bool b) {
	value.b = b;
	setType(eType_boolean);
}

void TValue::setFloat(const float n) {
	value.n = n;
	setType(eType_float);
}

void TValue::setString(const std::string s) {
	this->stringValue = s;
	setType(eType_string);
	// cout << "TValue::setString type been set to " << getType() << endl;
}

void TValue::setObject(void *p, const Scriptable::eType type) {
	value.p = p;
	objectType = type;
	setType(eType_Vobject);
}

void TValue::setPointer(void *p) {
	value.p = p;
}

const bool TValue::isInteger() const {
	return getType() == eType_integer;
}

const bool TValue::isFloat() const {
	return getType() == eType_float;
}

const bool TValue::isBoolean() const {
	return getType() == eType_boolean;
}

const bool TValue::isString() const {
	return getType() == eType_string;
}

const bool TValue::isObject() const {
	return getType() == eType_Vobject;
}

const bool TValue::isNil() const {
	return (getType() == eType_nil);
}

// Return a descriptive string value
const std::string TValue::toString() const {
	return getTypeName(getType()) + ": " + getValueAsString();
}
