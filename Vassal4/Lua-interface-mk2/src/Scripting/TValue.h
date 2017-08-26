/*
 * TValue.h
 *
 *  Created on: 09/08/2017
 *      Author: Brent
 */

#ifndef _TVALUE_H_
#define _TVALUE_H_

#include <Lua/luaconf.h>
#include <Scripting/Scriptable.h>
#include <string>

typedef union Value {
	void *p;		     /* Pointer to a Vassal object */
	bool b;			     /* boolean */
	LUA_INTEGER i;       /* integer number */
	LUA_NUMBER n; 		 /* float number */
} Value;

class TValue {
public:

	enum eType {
		eType_invalid,		// The TValue is invalid, it has not been initialised
		eType_any,			// Not a valid value for an actual TValue, used in parameter lists where the parameter type is unknown
		eType_integer,		// The TValue contains an integer value
		eType_float,		// The TValue contains a floating point value
		eType_boolean,		// The TValue contains a boolean value
		eType_string,		// The TValue contains a string value
		eType_nil,			// The TValue contains the nil value
		eType_Vobject		// The TValue contains a pointer to a Vassal Game Object of type objectType
	};

	static const eType eType_FIRST = eType_integer;
	static const eType eType_LAST = eType_Vobject;

	static std::string typeNames[];

	static std::string getTypeName(eType type) {
		if ((type >= eType_FIRST && type <= eType_LAST) || type == eType_invalid) {
			return typeNames[type];
		}
		return "Unknown";
	}

	// Constructors
	// Construct a nil TValue
	TValue();

	// Construct an Integer TValue
	TValue(int i);
	//TValue(long int i);
	//TValue(long long int i);

	// Construct a Float TValue
	// TValue(double n);
	TValue(float n);

	// Construct a Boolean TValue
	TValue(bool b);

	// Construct a String TValue
	TValue(std::string s);

	// Construct a Object TValue
	TValue(void *p, const Scriptable::eType objectType);
	TValue(void *p);

	virtual ~TValue();

	eType getType();

	// Return the value as a specific type
	// The Caller should have used isXxxxxx() to determine the correct type
	// TODO should we be auto-converting or throw an error?
	int getIntegerValue();
	bool getBooleanValue();
	float getFloatValue();
	std::string getStringValue();

	void * getObjectPointer();
	Scriptable::eType getObjectType();

	// Return the value as a string if possible.
	std::string getValueAsString();

	bool isNil();
	bool isInteger();
	bool isFloat();
	bool isBoolean();
	bool isString();
	bool isObject();

	// Return a descriptive string representation of the value (e.g. 'Integer: 2', 'Float: 3.4', 'String: Abc')
	const std::string toString();

	void setObjectType(const Scriptable::eType type);
private:

	void setType(const eType type);
	void setNil();
	void setInteger(const int i);
	void setBoolean(const bool b);
	void setFloat(const float n);
	void setString(const std::string s);
	void setObject(void *p, const Scriptable::eType type);
	void setPointer(void *p);

	eType valueType;
	Value value;
	std::string stringValue;
	Scriptable::eType objectType;
};

#endif /* _TVALUE_H_ */
