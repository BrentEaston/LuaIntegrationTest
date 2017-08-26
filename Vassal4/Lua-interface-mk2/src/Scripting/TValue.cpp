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

TValue::TValue(int i) {
	setInteger(i);
}

//TValue::TValue(long int i) {
//	setInteger(i);
//}

//TValue::TValue(long long int i) {
//	setInteger(i);
//}

//TValue::TValue(double n) {
//	setFloat(n);
//}

TValue::TValue(float n) {
	setFloat(n);
}

TValue::TValue(bool b) {
	setBoolean(b);
}
TValue::TValue(std::string s) {
	// cout << "TValue::TValue(std::string s)=" << s << endl;
	setString(s);
}

TValue::TValue(void *p, const Scriptable::eType objectType) {
	setObject(p, objectType);
}

TValue::TValue(void *p) {
	setPointer(p);
	setType(eType_Vobject);
}

TValue::eType TValue::getType() {
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
std::string TValue::getValueAsString() {
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
		return "";
		break;
	case eType_nil:
		return "";
		break;
	default:
		break;
	}

	return "Unknown";
}

int TValue::getIntegerValue() {
	return value.i;
}

bool TValue::getBooleanValue() {
	return value.b;
}

float TValue::getFloatValue() {
	return value.n;
}

std::string TValue::getStringValue() {
	return stringValue;
}

void * TValue::getObjectPointer() {
	return value.p;
}

Scriptable::eType TValue::getObjectType() {
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

bool TValue::isInteger() {
	return getType() == eType_integer;
}

bool TValue::isFloat() {
	return getType() == eType_float;
}

bool TValue::isBoolean() {
	return getType() == eType_boolean;
}

bool TValue::isString() {
	return getType() == eType_string;
}

bool TValue::isObject() {
	return getType() == eType_Vobject;
}

bool TValue::isNil() {
	return (getType() == eType_nil);
}

// Return a descriptive string value
const std::string TValue::toString() {
	return getTypeName(getType()) + ": " + getValueAsString();
}
